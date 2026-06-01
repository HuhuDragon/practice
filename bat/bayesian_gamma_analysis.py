#!/usr/bin/env python3
"""Read spectrum files and perform Bayesian template-weight analysis."""

from __future__ import annotations

import argparse
import csv
import math
import os
import random


def normalize(v: list[float]) -> list[float]:
    s = sum(v)
    return [x / s for x in v]


def read_templates_csv(path: str) -> tuple[list[float], list[list[float]]]:
    with open(path, "r", encoding="utf-8") as f:
        rows = list(csv.reader(f))
    if len(rows) < 2:
        raise ValueError("templates.csv is empty")

    header = rows[0]
    n_spectra = len(header) - 1
    if n_spectra < 2:
        raise ValueError("Need >=2 template spectra columns")

    energy: list[float] = []
    templates = [[] for _ in range(n_spectra)]
    for row in rows[1:]:
        energy.append(float(row[0]))
        for i in range(n_spectra):
            templates[i].append(float(row[i + 1]))

    templates = [normalize(s) for s in templates]
    return energy, templates


def read_measured_csv(path: str) -> tuple[list[float], list[int]]:
    with open(path, "r", encoding="utf-8") as f:
        rows = list(csv.reader(f))
    if len(rows) < 2:
        raise ValueError("measured.csv is empty")

    energy, counts = [], []
    for row in rows[1:]:
        energy.append(float(row[0]))
        counts.append(int(float(row[1])))
    return energy, counts


def log_posterior(weights: list[float], templates: list[list[float]], counts: list[int], alpha: float = 1.0) -> float:
    if any(w <= 0 for w in weights):
        return float("-inf")
    w = normalize(weights)

    total = sum(counts)
    ll = 0.0
    for b in range(len(counts)):
        lam = max(total * sum(w[i] * templates[i][b] for i in range(len(w))), 1e-12)
        ll += counts[b] * math.log(lam) - lam

    lp = (alpha - 1.0) * sum(math.log(max(x, 1e-12)) for x in w)
    return ll + lp


def run_mcmc(templates: list[list[float]], counts: list[int], n_steps: int, burn_in: int, proposal_scale: float, seed: int):
    rng = random.Random(seed)
    n_spectra = len(templates)
    w = normalize([rng.gammavariate(1.0, 1.0) for _ in range(n_spectra)])
    current = log_posterior(w, templates, counts)

    chain: list[list[float]] = []
    accepted = 0
    for t in range(n_steps):
        proposal = [w[i] + rng.gauss(0.0, proposal_scale) for i in range(n_spectra)]
        if all(x > 0 for x in proposal):
            proposal = normalize(proposal)
            cand = log_posterior(proposal, templates, counts)
            if math.log(rng.random()) < cand - current:
                w, current = proposal, cand
                accepted += 1
        if t >= burn_in:
            chain.append(w[:])
    return chain, accepted / n_steps


def percentile(sorted_vals: list[float], q: float) -> float:
    pos = (len(sorted_vals) - 1) * q
    lo, hi = int(math.floor(pos)), int(math.ceil(pos))
    if lo == hi:
        return sorted_vals[lo]
    frac = pos - lo
    return sorted_vals[lo] * (1 - frac) + sorted_vals[hi] * frac


def summarize(chain: list[list[float]]):
    n = len(chain[0])
    mean, lo, hi = [], [], []
    for i in range(n):
        col = [x[i] for x in chain]
        col_s = sorted(col)
        mean.append(sum(col) / len(col))
        lo.append(percentile(col_s, 0.025))
        hi.append(percentile(col_s, 0.975))
    return mean, lo, hi


def main() -> None:
    p = argparse.ArgumentParser(description="Read spectra files and run Bayesian analysis")
    # 可改：模板谱文件路径
    p.add_argument("--templates-csv", type=str, default="data/templates.csv")
    # 可改：测量谱文件路径
    p.add_argument("--measured-csv", type=str, default="data/measured.csv")
    # 可改：MCMC步数（增大更稳定但更慢）
    p.add_argument("--steps", type=int, default=4000)
    # 可改：烧入长度
    p.add_argument("--burn-in", type=int, default=1000)
    # 可改：提议分布宽度（过大接受率低，过小混合慢）
    p.add_argument("--proposal-scale", type=float, default=0.03)
    # 可改：Dirichlet先验参数
    p.add_argument("--alpha", type=float, default=1.0)
    # 可改：随机种子
    p.add_argument("--seed", type=int, default=44)
    # 可改：输出目录
    p.add_argument("--out-dir", type=str, default="output")
    args = p.parse_args()

    if args.burn_in >= args.steps:
        raise ValueError("--burn-in must be smaller than --steps")

    energy_t, templates = read_templates_csv(args.templates_csv)
    energy_m, counts = read_measured_csv(args.measured_csv)
    if len(energy_t) != len(energy_m):
        raise ValueError("template and measured bins mismatch")

    chain, acc = run_mcmc(templates, counts, args.steps, args.burn_in, args.proposal_scale, args.seed)
    mean, lo, hi = summarize(chain)

    print("=== BAT-like Bayesian Analysis ===")
    print(f"Templates: {len(templates)}, bins: {len(counts)}, acceptance: {acc:.3f}")
    for i in range(len(templates)):
        print(f"Spectrum {i+1}: posterior={mean[i]:.4f}, CI95=[{lo[i]:.4f}, {hi[i]:.4f}]")

    os.makedirs(args.out_dir, exist_ok=True)
    report = os.path.join(args.out_dir, "posterior_weights.csv")
    with open(report, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["spectrum_id", "posterior_mean", "ci95_low", "ci95_high"])
        for i in range(len(templates)):
            w.writerow([i + 1, mean[i], lo[i], hi[i]])

    total = sum(counts)
    recon = [total * sum(mean[i] * templates[i][b] for i in range(len(templates))) for b in range(len(counts))]

    try:
        import matplotlib.pyplot as plt

        plt.figure(figsize=(10, 5))
        plt.step(energy_m, counts, where="mid", label="measured", linewidth=1)
        plt.plot(energy_m, recon, label="BAT posterior reconstruction", alpha=0.9)
        plt.xlabel("Energy (keV)")
        plt.ylabel("Counts/bin")
        plt.title("Measured vs BAT reconstruction")
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(args.out_dir, "fit_comparison.png"), dpi=150)
        plt.close()
    except Exception as exc:
        print(f"[WARN] plotting skipped: {exc}")

    print("Saved:")
    print(" -", report)


if __name__ == "__main__":
    main()
