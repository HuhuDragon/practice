#!/usr/bin/env python3
"""Generate synthetic gamma spectra test files and save plots.

Usage example:
  python bat/generate_test_spectra.py --out-dir bat/data --n-spectra 3
"""

from __future__ import annotations

import argparse
import csv
import math
import os
import random


def gaussian(x: float, mu: float, sigma: float) -> float:
    z = (x - mu) / sigma
    return math.exp(-0.5 * z * z)


def normalize(v: list[float]) -> list[float]:
    s = sum(v)
    return [x / s for x in v]


def dirichlet_sample(n: int, alpha: float, rng: random.Random) -> list[float]:
    xs = [rng.gammavariate(alpha, 1.0) for _ in range(n)]
    return normalize(xs)


def poisson_sample(lam: float, rng: random.Random) -> int:
    if lam <= 0:
        return 0
    l = math.exp(-lam)
    k, p = 0, 1.0
    while p > l:
        k += 1
        p *= rng.random()
    return k - 1


def make_templates(n_spectra: int, n_bins: int, seed: int) -> tuple[list[float], list[list[float]]]:
    rng = random.Random(seed)
    energy = [3000.0 * i / (n_bins - 1) for i in range(n_bins)]
    centers = [350 + (2600 - 350) * i / (n_spectra - 1) for i in range(n_spectra)]
    templates: list[list[float]] = []
    for i, c in enumerate(centers):
        sigma_main = rng.uniform(30, 90)
        sigma_second = rng.uniform(40, 120)
        second_center = c * rng.uniform(0.65, 0.9)
        decay = rng.uniform(1200, 2200)
        wave = rng.uniform(80, 180)
        spec = []
        for e in energy:
            main = gaussian(e, c, sigma_main)
            second = 0.35 * gaussian(e, second_center, sigma_second)
            continuum = 0.08 * math.exp(-e / decay)
            wiggle = 1.0 + 0.03 * math.sin(e / wave + i)
            spec.append(max((main + second + continuum) * wiggle, 0.0))
        templates.append(normalize(spec))
    return energy, templates


def save_templates(path: str, energy: list[float], templates: list[list[float]]) -> None:
    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        header = ["energy_keV"] + [f"template_{i+1}" for i in range(len(templates))]
        w.writerow(header)
        for b, e in enumerate(energy):
            row = [e] + [templates[i][b] for i in range(len(templates))]
            w.writerow(row)


def save_measured(path: str, energy: list[float], measured: list[int]) -> None:
    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["energy_keV", "measured_counts"])
        for e, c in zip(energy, measured):
            w.writerow([e, c])


def save_truth(path: str, weights: list[float]) -> None:
    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["spectrum_id", "true_weight"])
        for i, v in enumerate(weights, start=1):
            w.writerow([i, v])


def main() -> None:
    p = argparse.ArgumentParser(description="Generate synthetic spectra files and plots")
    # 可改：模拟模板谱个数（至少2）
    p.add_argument("--n-spectra", type=int, default=3)
    # 可改：能道数（越大越细）
    p.add_argument("--bins", type=int, default=256)
    # 可改：总计数，影响统计涨落大小
    p.add_argument("--total-counts", type=int, default=20000)
    # 可改：随机种子，便于复现
    p.add_argument("--seed", type=int, default=42)
    # 可改：输出目录
    p.add_argument("--out-dir", type=str, default="bat/data")
    args = p.parse_args()

    if args.n_spectra < 2:
        raise ValueError("--n-spectra must be >= 2")
    os.makedirs(args.out_dir, exist_ok=True)

    energy, templates = make_templates(args.n_spectra, args.bins, args.seed)
    rng = random.Random(args.seed + 1)
    true_weights = dirichlet_sample(args.n_spectra, 1.5, rng)

    expected = []
    for b in range(args.bins):
        mix = sum(true_weights[i] * templates[i][b] for i in range(args.n_spectra))
        expected.append(args.total_counts * mix)
    measured = [poisson_sample(x, rng) for x in expected]

    templates_csv = os.path.join(args.out_dir, "templates.csv")
    measured_csv = os.path.join(args.out_dir, "measured.csv")
    truth_csv = os.path.join(args.out_dir, "truth_weights.csv")
    save_templates(templates_csv, energy, templates)
    save_measured(measured_csv, energy, measured)
    save_truth(truth_csv, true_weights)

    try:
        import matplotlib.pyplot as plt

        plt.figure(figsize=(10, 5))
        for i, s in enumerate(templates):
            plt.plot(energy, s, label=f"template_{i+1}")
        plt.xlabel("Energy (keV)")
        plt.ylabel("Normalized intensity")
        plt.title("Synthetic template spectra")
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(args.out_dir, "templates.png"), dpi=150)
        plt.close()

        plt.figure(figsize=(10, 5))
        plt.step(energy, measured, where="mid", label="measured", linewidth=1)
        plt.plot(energy, expected, label="expected (truth mixture)", alpha=0.8)
        plt.xlabel("Energy (keV)")
        plt.ylabel("Counts/bin")
        plt.title("Synthetic measured spectrum")
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(args.out_dir, "measured.png"), dpi=150)
        plt.close()
    except Exception as exc:
        print(f"[WARN] plotting skipped: {exc}")

    print("Generated files:")
    print(" -", templates_csv)
    print(" -", measured_csv)
    print(" -", truth_csv)


if __name__ == "__main__":
    main()
