# script for comparing egamma and hadron parameters
import argparse
import glob
import numpy as np
import pandas as pd
import uproot
import matplotlib.pyplot as plt
import os
from make_discr import total_reso, default_reso, default_noise, constant_term

parser = argparse.ArgumentParser(add_help = False)

parser.add_argument("-h", "--histos", action = "store_true")
parser.add_argument("-s", "--scatter", action = "store_true")
parser.add_argument("-b", "--box", action = "store_true")

args = parser.parse_args()

plt.rc('axes', axisbelow=True)

eg_col = "purple"
had_col = "darkorange"

point_alpha = 1
point_size = 0.01


def mkdir(path: str):
    if not os.path.exists(path):
        os.makedirs(path)

def add_histo_1d(counts, bin_edges, **step_kw):
    x = np.concatenate((np.array([bin_edges[0]]), bin_edges))
    y = np.concatenate((np.array([0]), counts, np.array([0])))

    plt.step(x, y, **step_kw) 

def make_histos(
    df_eg: pd.DataFrame, df_had: pd.DataFrame, 
    var: str, save_to: str, xlabel: str = ""
):
    counts_eg, bin_edges_eg = np.histogram(df_eg[var], bins = 100)
    counts_had, bin_edges_had = np.histogram(df_had[var], bins = 100)

    _, ax = plt.subplots(1,1, figsize = (6, 5.5))
    plt.grid(alpha = 0.5)

    add_histo_1d(
        counts_eg, bin_edges_eg, color = eg_col, label = "EG",
        where = "pre", linewidth = 1.2
    )
    add_histo_1d(
        counts_had, bin_edges_had, color = had_col, label = "HAD",
        where = "pre", linewidth = 1.2
    )
    
    plt.xlabel(f"{var}{xlabel}")
    plt.ylabel("count")

    plt.legend()
    plt.tight_layout()

    plt.savefig(save_to)

base_dir = "img/comp/flat"
mkdir(base_dir)

with uproot.open("data/flat/data_05.root") as fin:
    for key, tree in fin.items():
        df = tree.arrays(library="pd")

        df_eg = df.query("pid == 0")
        df_had = df.query("pid == 1")
        
        if args.histos:
            make_histos(
                df_eg, df_had, "long_mean", f"{base_dir}/long_mean.png",
                xlabel = " (cells)"
            )

            make_histos(
                df_eg, df_had, "lat_mean", f"{base_dir}/lat_mean.png",
                xlabel = " (cells)"
            )

            make_histos(
                df_eg, df_had, "energy", f"{base_dir}/energy.png",
                xlabel = " (GeV)"
            )
        
        if args.scatter:
            _ = plt.figure(figsize=(6, 5.5))
            
            plt.grid(alpha=0.5)
            plt.scatter(
                "energy",
                "long_mean",
                data=df_eg,
                color=eg_col,
                alpha=point_alpha,
                s=point_size,
                label='EG'
            )
            plt.scatter(
                "energy",
                "long_mean",
                data=df_had,
                color=had_col,
                alpha=point_alpha,
                s=point_size,
                label='HAD'
            )

            plt.xlabel("Energy (GeV)")
            plt.ylabel("long_mean (cells)")

            plt.legend(markerscale = 50)
            plt.tight_layout()

            plt.savefig(f"{base_dir}/energyVlong.png")


if args.box:
    sqrt10 = np.sqrt(10)
    reso10 = [
        total_reso(default_reso * (sqrt10 ** val), default_noise * (sqrt10 ** val), constant_term, 10)
        for val in range(-5, 7)
    ]

    eg_long_means = []
    had_long_means = []
    i = 0
    for input_path in glob.iglob("data/discr/flat/*.root"):
        with uproot.open(input_path) as input_file:
            for key, tree in input_file.items():
                df = tree.arrays(library = "pd", filter_name = ["long_mean", "pid"])

                eg_long_means.append(df.query("pid==0")["long_mean"])
                had_long_means.append(df.query("pid==1")["long_mean"])

                i += 1

    positions = np.arange(i) * 2.2
    offset = 0.25
    _, ax = plt.subplots(1,1, figsize = (12, 5.5))
    plt.grid(alpha = 0.5)

    beg = ax.boxplot(
        eg_long_means, positions = positions - offset, showfliers = False,
        patch_artist = True
    )
    bhad = ax.boxplot(
        had_long_means, positions = positions + offset, showfliers = False,
        patch_artist = True
    )

    for eg_patch, had_patch in zip(beg["boxes"], bhad["boxes"]):
        eg_patch.set_facecolor(eg_col)
        had_patch.set_facecolor(had_col)

    for eg_med, had_med in zip(beg["medians"], bhad["medians"]):
        eg_med.set_color(had_col)
        had_med.set_color(eg_col)

    plt.xticks(positions)
    ax.set_xticklabels([f"{r:.1f}" for r in reso10])
    
    plt.scatter([], [], c = eg_col, label = "EG")
    plt.scatter([], [], c = had_col, label = "HAD")

    plt.legend()

    plt.xlabel("Resolution at 10 GeV (%)")
    plt.ylabel("long_mean (cells)")
    plt.tight_layout()

    plt.savefig(f"{base_dir}/long_mean_box.png")



