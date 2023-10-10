# script for comparing egamma and hadron parameters

import pandas as pd
import uproot
import matplotlib.pyplot as plt
import os

plt.rc('axes', axisbelow=True)

eg_col = "darkblue"
had_col = "red"

point_alpha = 0.05
point_size = 2


def mkdir(path: str):
    if not os.path.exists(path):
        os.makedirs(path)


with uproot.open("../data/flat/data_03.root") as fin:
    base_dir = "../img/comp"
    mkdir(base_dir)

    for key, tree in fin.items():
        df = tree.arrays(library="pd")

        df_eg = df.query("pid == 0")
        df_had = df.query("pid == 1")

        _ = plt.figure(figsize=(6, 5))
        
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
        plt.ylabel("Longitudinal Mean (cells)")

        plt.legend()
        plt.tight_layout()

        plt.savefig(f"{base_dir}/evlong.png")
