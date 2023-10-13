import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from make_discr import total_reso, default_reso, default_noise, constant_term

input_dir = "data/discr"
output_dir = "img/discr"
target_efficiency = 0.992

sqrt10 = np.sqrt(10)

dfr = pd.read_csv(f"{input_dir}/dbr.csv")
dfr["reso10"] = [
    total_reso(default_reso * (sqrt10 ** val), default_noise * (sqrt10 ** val), constant_term, 10)
    for val in dfr["value"]
]

dfe = pd.read_csv(f"{input_dir}/dbe.csv")

color = "purple"


_, axr = plt.subplots(1,1, figsize = (6, 5.5))
plt.grid(alpha = 0.5)

plt.scatter("reso10", "bkg_eff", data = dfr, color = color, s = 17)

plt.title(f"signal efficiency = {target_efficiency}, flat energy at [2,100]GeV", loc = "left")
plt.xlabel("resolution at 10GeV (%)")
plt.ylabel("background efficiency")

ticks_r = [1, 10, 100]

plt.xscale("log")
plt.xticks(ticks_r)
axr.set_xticklabels(ticks_r)
plt.tight_layout()

plt.savefig(f"{output_dir}/resoVeff.png")

#----------------------------------------------------------------------

_, axe = plt.subplots(1,1, figsize = (6, 5.5))
plt.grid(alpha = 0.5)

plt.scatter("value", "bkg_eff", data = dfe, color = color, s = 17)

plt.title(f"signal efficiency = {target_efficiency}, resolution = 5%@10GeV", loc = "left")
plt.xlabel("Generated particle energy (GeV)")
plt.ylabel("background efficiency")

ticks_e = [5, 10, 100]

plt.xscale("log")
plt.xticks(ticks_e)
axe.set_xticklabels(ticks_e)
plt.tight_layout()

plt.savefig(f"{output_dir}/energyVeff.png")



