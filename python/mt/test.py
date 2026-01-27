


import numpy as np
import pandas as pd 
import matplotlib.pyplot as plt
from scipy.stats import chi2



def narrow_res_rate(omega_gamma, Er, mu, T9):

    return 1.5399e11 * omega_gamma * mu**(-1.5) * T9**(-1.5) * np.exp(-11.605 * Er / T9)

def sample_PT_truncated(mean_val, UL, size):

    y_UL = UL / mean_val
    cdf_UL = chi2.cdf(y_UL, df=1)
    u = np.random.random(size=size) * cdf_UL
    y = chi2.ppf(u, df=1)
    return mean_val * y

def sample_log_uniform(UL, low_factor=1e-6, size=1):

    log_low = np.log10(UL * low_factor)
    log_high = np.log10(UL)
    return 10 ** np.random.uniform(log_low, log_high, size)


Er = 0.09          # 共振能量
mu = 0.962962963            # 约化质量 
omega_gamma_UL = 1.20E-10  # 上限 
omega_gamma_mean = 1.20E-11  # 期望值
Nsamp = 20000     
T9_grid = np.logspace(-2, 1, 60)  # 0.01 - 10 GK


print("Sampling...")
omega_PT = sample_PT_truncated(omega_gamma_mean, omega_gamma_UL, Nsamp)
omega_LU = sample_log_uniform(omega_gamma_UL, low_factor=1e-6, size=Nsamp)


omega_fixed01 = 0.1 * omega_gamma_UL


print("Computing rates...")
rates_PT = np.array([narrow_res_rate(omega_PT, Er, mu, T9) for T9 in T9_grid])
rates_LU = np.array([narrow_res_rate(omega_LU, Er, mu, T9) for T9 in T9_grid])
rate_fixed01 = np.array([narrow_res_rate(omega_fixed01, Er, mu, T9) for T9 in T9_grid])



def summarize(arr):
    return {
        "median": np.median(arr),
        "p16": np.percentile(arr, 16),
        "p84": np.percentile(arr, 84)
    }

df_PT = pd.DataFrame([summarize(rates_PT[i, :]) for i in range(len(T9_grid))])
df_LU = pd.DataFrame([summarize(rates_LU[i, :]) for i in range(len(T9_grid))])
df_PT["T9"] = df_LU["T9"] = T9_grid


plt.figure(figsize=(6,4))
plt.hist(omega_PT, bins=200, alpha=0.5, label="PT (truncated)")
plt.hist(omega_LU, bins=200, alpha=0.5, label="Log-uniform")
plt.axvline(omega_fixed01, color='r', ls='--', label='0.1×UL')

plt.axvline(omega_gamma_UL, color='grey', ls=':', label='UL')
plt.xlabel(r'$\omega\gamma$ [MeV]')
plt.ylabel('Samples')
plt.legend()
plt.title('Sampled ωγ Distributions')
plt.tight_layout()
plt.savefig("omega_gamma_samples.png", dpi=200)


plt.figure(figsize=(6,4))
plt.fill_between(T9_grid, df_PT["p16"], df_PT["p84"], alpha=0.3)
plt.plot(T9_grid, df_PT["median"], '-', label="Monte Carlo")
plt.plot(T9_grid, rate_fixed01, ':', label="0.1×UL")
plt.xscale('log')
plt.yscale('log')
plt.xlabel("$T_9$ (GK)")
plt.ylabel(r"$N_A\langle\sigma v\rangle$ (cm$^3$ s$^{-1}$ mol$^{-1}$)")
plt.legend()
plt.tight_layout()
plt.savefig("rate_vs_T9.png", dpi=200)


T9_sel = 0.1
idx_sel = np.argmin(np.abs(T9_grid - T9_sel))
print(f"\nNumeric example at T9={T9_sel:.2f} GK:")
print(f"PT: median={df_PT['median'][idx_sel]:.3e}, p16={df_PT['p16'][idx_sel]:.3e}, p84={df_PT['p84'][idx_sel]:.3e}")
print(f"Log-uniform: median={df_LU['median'][idx_sel]:.3e}, p16={df_LU['p16'][idx_sel]:.3e}, p84={df_LU['p84'][idx_sel]:.3e}")
print(f"Fixed 0.1×UL: {rate_fixed01[idx_sel]:.3e}")



out_df = pd.DataFrame({
    "T9": T9_grid,
    "PT_median": df_PT["median"],
    "PT_p16": df_PT["p16"],
    "PT_p84": df_PT["p84"],
    "LU_median": df_LU["median"],
    "LU_p16": df_LU["p16"],
    "LU_p84": df_LU["p84"],
    "Fixed01": rate_fixed01,
})
out_df.to_csv("rate_results.csv", index=False)
print("\nResults saved: rate_results.csv, omega_gamma_samples.png, rate_vs_T9.png")
