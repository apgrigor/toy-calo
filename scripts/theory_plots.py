import argparse
import numpy as np
import matplotlib.pyplot as plt
import profiles
from make_discr import total_reso, default_reso, default_noise, constant_term

output_dir = "img/theory"

parser = argparse.ArgumentParser()

parser.add_argument("-e", "--energy", action = "store_true")
parser.add_argument("-l", "--longitudinal", action = "store_true")
parser.add_argument("-a", "--lateral", action = "store_true")


args = parser.parse_args()

colors = ["purple", "orchid", "darkorange"]

energies = np.linspace(1, 60, 120)

offset = 10

axvline_linewidth = 0.8

if args.energy:
    default_resos = np.array([
        total_reso(default_reso, default_noise, constant_term, energy)
        for energy in energies
    ])
    high_resos = np.array([
        total_reso(default_reso / offset, default_noise / offset, constant_term, energy)
        for energy in energies
    ])
    low_resos = np.array([
        total_reso(default_reso * offset, default_noise * offset, constant_term, energy)
        for energy in energies
    ])
    _, ax = plt.subplots(1,1, figsize = (6, 5.5))
    plt.grid(alpha = 0.5)
    
    plt.plot(
        energies, high_resos, color = colors[0],
        label = f"better resolution  (~{high_resos[20]:.0f}%@10GeV)"
    )
    plt.plot(
        energies, default_resos, color = colors[1],
        label = f"default resolution (~{default_resos[20]:.0f}%@10GeV)"
    )
    plt.plot(
        energies[2:], low_resos[2:], color = colors[2],
        label = f"worse resolution (~{low_resos[20]:.0f}%@10GeV)"
    )
    
    plt.legend()
    plt.axvline(10, color = "black", linestyle = "--", linewidth = axvline_linewidth)
    
    plt.xlabel("energy (GeV)")
    plt.ylabel("resolution (%)")

    plt.tight_layout()
    plt.savefig(f"{output_dir}/energyVreso.png")


def draw_profile(profile_func, save_to: str, title=None, annot_y = 0.335):
    _, _ = plt.subplots(1,1, figsize = (6, 5.5))
    plt.grid(alpha=0.5)

    plt.xlabel("depth (cells)")
    plt.ylabel("energy deposition (arbitrary units)")
    plt.title(title)

    xs = np.linspace(0, 9, 180)

    energies = [5, 20,100]

    for energy, color in zip(energies, colors):
        ys = profile_func(energy, xs)

        plt.plot(xs, ys, color = color, label = f"{energy} GeV")
    
    plt.axvline(4, color = "black", linestyle = "--", linewidth = axvline_linewidth)
    plt.annotate("EG CALO", (2.5, annot_y))
    plt.axvline(9, color = "black", linestyle = "--", linewidth = axvline_linewidth)
    plt.annotate("HAD CALO", (4.3, annot_y))

    plt.legend()
    plt.tight_layout()
    plt.savefig(save_to)

if args.longitudinal:
    draw_profile(
        profiles.egamma_profile,
        f"{output_dir}/egamma_profile.png",
        title = "EG longitudinal profiles"
    )

    draw_profile(
        profiles.hadron_profile,
        f"{output_dir}/hadron_profile.png",
        title = "HAD longitudinal profiles",
        annot_y = 0.113
    )


def draw_lat_profile(R0_func, title, save_to):
    energies = [5, 20, 100]
    ts = np.linspace(0, 9, 60)
    critical_point = 0.95
    coeff = np.sqrt(1 / (1 - critical_point) - 1)
    max_radius = 4

    _, _ = plt.subplots(1,1, figsize = (6, 5.5))
    plt.grid(alpha=0.5)

    plt.xlabel("depth (cells)")
    plt.ylabel(f"Radius containing {critical_point} of the probability")
    plt.title(title)
    
    for energy, color in zip(energies, colors):
        r0s = np.array([R0_func(energy, t) for t in ts])
        r0s2 = r0s ** 2

        norms = 1 - r0s2 / (max_radius ** 2 + r0s2)

        coeffs = np.sqrt(1 / (1 - norms * critical_point) - 1)

        plt.plot(ts, r0s * coeffs, color = color, label = f"{energy} GeV")
    
    plt.axvline(4, color = "black", linestyle = "--", linewidth = axvline_linewidth)
    plt.axvline(9, color = "black", linestyle = "--", linewidth = axvline_linewidth)

    plt.legend()
    plt.tight_layout()
    plt.savefig(save_to)

if args.lateral:
    draw_lat_profile(
        lambda E, t: 0.5 + 0.05 * np.log(E) * t,
        "EG lateral profiles",
        f"{output_dir}/egamma_lat_profile.png"
    )

    draw_lat_profile(
        lambda E, t: (0.5 + 0.1 * np.log(E) * t) ** 2,
        "HAD lateral profiles",
        f"{output_dir}/hadron_lat_profile.png"
    )














