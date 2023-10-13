import numpy as np
import matplotlib.pyplot as plt
from scipy.special import gamma
from dataclasses import dataclass


# longitudinal profiles (energy dependent)
@dataclass
class GammaParams:
    a: float
    b: float

    @classmethod
    def egamma(cls, energy):
        log_energy = np.log(energy)

        mode = 0.65 * log_energy - 0.45
        a = 1.0 * log_energy + 0.5

        return GammaParams(a, (a - 1) / mode)

    @classmethod
    def hadron(cls, energy):
        log_energy = np.log(energy)

        mode = 1.1 * log_energy + 0.3
        a = 3.5 * log_energy + 7.5

        return GammaParams(a, (a - 1) / mode)


def gamma_dist(p: GammaParams, x):
    return (p.b * x) ** (p.a - 1) * np.exp(-p.b * x) / gamma(p.a)


def egamma_profile(energy, xs):
    eparams = GammaParams.egamma(energy)

    func = np.vectorize(lambda x: gamma_dist(eparams, x))

    return func(xs)


def hadron_profile(energy, xs):
    c = 0.33

    eparams = GammaParams.egamma(energy)
    hparams = GammaParams.hadron(energy)

    func = np.vectorize(
        lambda x: c * gamma_dist(eparams, x) + (1 - c) * gamma_dist(hparams, x)
    )

    return func(xs)


def draw_profiles(profile_func, axis, title=None):
    axis.grid(alpha=0.5)
    axis.set_xlabel("depth (cells)")
    axis.set_ylabel("energy deposition (arbitrary units)")
    axis.set_title(title)

    xs = np.linspace(0, 9, 180)

    energies = [5, 10, 20, 50, 100]
    colors = ["black", "darkred", "firebrick", "red", "orange"]

    for energy, color in zip(energies, colors):
        ys = profile_func(energy, xs)

        axis.plot(xs, ys, color=color, label=f"{energy} GeV")

    axis.legend()

if __name__ == "__main__":
    # fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
    
    fig1, ax1 = plt.subplots(1, 1, figsize=(6, 5))
    fig2, ax2 = plt.subplots(1, 1, figsize=(6, 5))
    
    
    draw_profiles(egamma_profile, ax1, title="EG")
    draw_profiles(hadron_profile, ax2, title="HAD")
    
    # fig.tight_layout()
    # fig.savefig("../img/profiles/long_0.png")
    
    fig1.tight_layout()
    fig2.tight_layout()
    
    fig1.savefig("../img/profiles/long_0_eg.png")
    fig2.savefig("../img/profiles/long_0_had.png")
