# script for comparing egamma and hadron parameters

import pandas as pd
import uproot
import matplotlib.pyplot as plt
import os


def mkdir(path: str):
    if not os.path.exists(path):
        os.makedirs(path)


def save_comp(
    quantity: str,
    df_eg: pd.DataFrame,
    df_had: pd.DataFrame,
    path: str,
    energy=None,
    bins=None,
    rng=None,
):
    _, (ax1, ax2) = plt.subplots(2, 1, figsize=(6, 5))

    ax1.set_axisbelow(True)
    ax2.set_axisbelow(True)

    ax1.grid(zorder=0, alpha=0.5)
    ax2.grid(zorder=0, alpha=0.5)

    ax1.hist(quantity, data=df_eg, color="darkblue", bins=bins, range=rng)
    ax2.hist(quantity, data=df_had, color="red", bins=bins, range=rng)

    ax1.set_title("EG", loc="left")
    ax2.set_title("HAD", loc="left")

    ax1.set_ylabel("count")
    ax2.set_ylabel("count")

    if energy is None:
        ax2.set_xlabel(quantity)
    else:
        ax2.set_xlabel(f"{quantity} at {energy} GeV")

    plt.tight_layout()
    plt.savefig(path)
    plt.close()


def save_comps(df_eg, df_had, dirname, energy=None):
    save_comp("long_mean", df_eg, df_had, f"{dirname}/long_mean.png", energy, 100, (0, 8))
    save_comp("lat_mean", df_eg, df_had, f"{dirname}/lat_mean.png", energy, 100, (0, 1))
    save_comp("depth_m1", df_eg, df_had, f"{dirname}/depth_m1.png", energy, 8, (0, 8))
    save_comp("depth_m2", df_eg, df_had, f"{dirname}/depth_m2.png", energy, 8, (0, 8))
    save_comp("em_frac", df_eg, df_had, f"{dirname}/em_frac.png", energy, 100, (0, 100))

sample = "flat"

with uproot.open(f"../data/{sample}/data_03.root") as fin:
    base_dir = f"../img/comp/{sample}"
    mkdir(base_dir)

    for key, tree in fin.items():
        df = tree.arrays(library="pd")

        df_eg = df.query("pid == 0")
        df_had = df.query("pid == 1")

        save_comps(df_eg, df_had, base_dir)

        for energy in [5, 10, 20, 50, 100]:
            energy_query = f"(energy>{energy-1})&(energy<{energy+1})"

            df_eg_q = df_eg.query(energy_query)
            df_had_q = df_had.query(energy_query)

            dirname = f"{base_dir}/e{energy}"
            mkdir(dirname)

            save_comps(df_eg_q, df_had_q, dirname, energy)
