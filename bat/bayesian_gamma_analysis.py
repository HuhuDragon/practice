#!/usr/bin/env python3
"""Bayesian decomposition for gamma spectra using multiple simulated templates."""

from __future__ import annotations

import argparse
import math
import random
from dataclasses import dataclass


@dataclass
class SpectrumData:
    energy: list[float]
    templates: list[list[float]]
    measured_counts: list[int]
    true_weights: list[float]


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
    # Knuth algorithm; good for moderate lambda (sufficient for demo/testing)
    if lam <= 0:
        return 0
    l = math.exp(-lam)
    k = 0
    p = 1.0
    while p > l:
        k += 1
        p *= rng.random()
    return k - 1


def make_templates(n_spectra: int, n_bins: int, seed: int) -> tuple[list[float], list[list[float]]]:
    if n_spectra < 2:
        raise ValueError("n_spectra must be >= 2")
    rng = random.Random(seed)

    energy = [3000.0 * i / (n_bins - 1) for i in range(n_bins)]
    templates: list[list[float]] = []

    centers = [350 + (2600 - 350) * i / (n_spectra - 1) for i in range(n_spectra)]

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
            value = max((main + second + continuum) * wiggle, 0.0)
            spec.append(value)

        templates.append(normalize(spec))

    return energy, templates


def simulate_measurement(templates: list[list[float]], total_counts: int, seed: int) -> tuple[list[int], list[float]]:
    rng = random.Random(seed)
    n_spectra = len(templates)
    n_bins = len(templates[0])

    true_weights = dirichlet_sample(n_spectra, alpha=1.5, rng=rng)

    expected = []
    for b in range(n_bins):
        mix = 0.0
        for i in range(n_spectra):
            mix += true_weights[i] * templates[i][b]
        expected.append(total_counts * mix)

    measured = [poisson_sample(max(x, 1e-12), rng) for x in expected]
    return measured, true_weights


def log_posterior(weights: list[float], templates: list[list[float]], counts: list[int], alpha: float = 1.0) -> float:
    if any(w <= 0 for w in weights):
        return float("-inf")

    w = normalize(weights)
    total = sum(counts)
    n_bins = len(counts)

    ll = 0.0
    for b in range(n_bins):
        lam = 0.0
        for i in range(len(w)):
            lam += w[i] * templates[i][b]
        lam = max(total * lam, 1e-12)
        ll += counts[b] * math.log(lam) - lam

    lp = (alpha - 1.0) * sum(math.log(max(x, 1e-12)) for x in w)
    return ll + lp


def run_mcmc(templates: list[list[float]], counts: list[int], n_steps: int, burn_in: int, proposal_scale: float, seed: int):
    rng = random.Random(seed)
    n_spectra = len(templates)

    w = dirichlet_sample(n_spectra, alpha=1.0, rng=rng)
    current = log_posterior(w, templates, counts)

    chain: list[list[float]] = []
    accepted = 0

    for t in range(n_steps):
        proposal = [w[i] + rng.gauss(0.0, proposal_scale) for i in range(n_spectra)]
        if all(x > 0 for x in proposal):
            proposal = normalize(proposal)
            cand = log_posterior(proposal, templates, counts)
            if math.log(rng.random()) < (cand - current):
                w, current = proposal, cand
                accepted += 1
        if t >= burn_in:
            chain.append(w[:])

    return chain, accepted / n_steps


def percentile(sorted_vals: list[float], q: float) -> float:
    if not sorted_vals:
        return float("nan")
    pos = (len(sorted_vals) - 1) * q
    lo = int(math.floor(pos))
    hi = int(math.ceil(pos))
    if lo == hi:
        return sorted_vals[lo]
    frac = pos - lo
    return sorted_vals[lo] * (1 - frac) + sorted_vals[hi] * frac


def summarize(chain: list[list[float]]):
    n = len(chain[0])
    means, los, his = [], [], []
    for i in range(n):
        col = [row[i] for row in chain]
        col_sorted = sorted(col)
        means.append(sum(col) / len(col))
        los.append(percentile(col_sorted, 0.025))
        his.append(percentile(col_sorted, 0.975))
    return means, los, his


def main() -> None:
    p = argparse.ArgumentParser(description="Bayesian analysis for gamma spectra from multiple simulated templates")
    p.add_argument("--n-spectra", type=int, default=3)
    p.add_argument("--bins", type=int, default=256)
    p.add_argument("--total-counts", type=int, default=20000)
    p.add_argument("--steps", type=int, default=4000)
    p.add_argument("--burn-in", type=int, default=1000)
    p.add_argument("--proposal-scale", type=float, default=0.03)
    p.add_argument("--seed", type=int, default=42)
    args = p.parse_args()

    if args.n_spectra < 2:
        raise ValueError("--n-spectra must be >= 2")
    if args.burn_in >= args.steps:
        raise ValueError("--burn-in must be smaller than --steps")

    energy, templates = make_templates(args.n_spectra, args.bins, args.seed)
    measured, true_weights = simulate_measurement(templates, args.total_counts, args.seed + 1)
    chain, acc = run_mcmc(templates, measured, args.steps, args.burn_in, args.proposal_scale, args.seed + 2)
    mean, lo, hi = summarize(chain)

    print("=== Bayesian Gamma Spectrum Decomposition ===")
    print(f"Template spectra count: {args.n_spectra}")
    print(f"Energy bins: {len(energy)}")
    print(f"MCMC acceptance rate: {acc:.3f}\n")
    for i in range(args.n_spectra):
        print(f"Spectrum {i+1}: true={true_weights[i]:.4f}, posterior={mean[i]:.4f}, CI95=[{lo[i]:.4f}, {hi[i]:.4f}]")


if __name__ == "__main__":
    main()
