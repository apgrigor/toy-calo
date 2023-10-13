import argparse
import subprocess
import numpy as np
import pprint as pp

def total_reso(reso, noise, const, energy):
    return 100 * np.sqrt(
        reso / energy + noise / energy**2 + const
    )

default_reso = 0.05 ** 2
default_noise = 0.5 ** 2
constant_term = 0.0055 ** 2

sqrt10 = np.sqrt(10)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    
    parser.add_argument("-r", "--reso", action = "store_true")
    parser.add_argument("-e", "--energy", action = "store_true")
    
    args = parser.parse_args()

    params = [
        (default_reso * (sqrt10 ** order), default_noise * (sqrt10 ** order))
        for order in range(-5, 7)
    ]
    
    total_resos = [
        (i - 5, total_reso(reso, noise, constant_term, 10))
        for i, (reso, noise) in enumerate(params)
    ]
    
    # pp.pprint(total_resos)
    
    energies = [3, 4, 5, 6, 7, 8, 10, 20, 35, 50, 75, 100]
    
    if args.reso:
        print("making distinct resolution samples")
        for reso, noise in params:
            subprocess.run([
                "xmake", "run", "toy", "flat", "discr",
                str(reso), str(noise)
            ])
    
    if args.energy:
        print("making energy samples")
        for energy in energies:
            subprocess.run([
                "xmake", "run", "toy", "exact", "discr",
                str(default_reso), str(default_noise), str(energy)
            ])
