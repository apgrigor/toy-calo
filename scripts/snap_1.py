import os
import uproot
import matplotlib.pyplot as plt

def mkdir(path: str):
    if not os.path.exists(path):
        os.makedirs(path)

with uproot.open("../data/flat_spec/snap_03.root") as fin:
    for key, tree in fin.items():
        arr = tree.arrays(library="ak", filter_name=["sim_hits", "energy", "pid"])

        for i, sim_hits in enumerate(arr):
            # print(sim_hits['pid'])

            sh_slice = sim_hits["sim_hits"].to_numpy().sum(axis=2)
            # sh_slice = sim_hits['sim_hits'].to_numpy()[:,:,4]

            # print(type(sim_hits['sim_hits'].to_numpy()))
            plt.figure(figsize=(6, 5))
            plt.pcolormesh(sh_slice, cmap="hot")
            cbar = plt.colorbar()
            cbar.set_label("energy (GeV)")
            plt.xlabel("eta")
            plt.ylabel("depth")

            energy = f"{sim_hits['energy']:.0f}"

            plt.title(
                f'{"EG" if sim_hits["pid"] == 0 else "HAD"} (total energy = {energy} GeV)'
            )

            mkdir(f"../img/snap/flat_spec/e{energy}")

            plt.tight_layout()
            plt.savefig(f"../img/snap/flat_spec/e{energy}/snap_{i:02}_mesh")
            plt.close()

            # plt.figure(figsize=(6, 5))
            # plt.grid(alpha=0.5)
            # plt.bar(range(8), sh_slice.sum(axis=1), color="red")
            # plt.title(f'energy={sim_hits["energy"]:.2f} GeV')

            # plt.tight_layout()
            # plt.savefig(f"../img/snap/snap_{i:02}_depth")
            # plt.close()
