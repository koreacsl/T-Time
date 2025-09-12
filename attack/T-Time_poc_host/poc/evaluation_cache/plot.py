import numpy as np
import matplotlib.pyplot as plt
import sys

plt.figure(figsize=(12, 8))

max_y_value = 0

labels = ["NUM=0", "NUM=1"]
linestyles = ["-", "-"]
colors = ["C0", "C1"]  

for idx, input in enumerate(sys.argv[1:]):
    data = np.loadtxt(input, delimiter=';')
    hist, bins = np.histogram(data, bins=300)

    print(f"File: {input}")
    print(f"Number of bins: {len(bins) - 1}")

    max_y_value = max(max_y_value, max(hist))

    plt.plot(
        bins[:-1],
        hist,
        linestyle=linestyles[idx],
        color=colors[idx],       
        label=labels[idx]
    )

plt.ylabel("Frequency", fontsize=22)
plt.xlabel("Dwell time (cycles)", fontsize=22)

yticks = plt.yticks()[0]
yticks = [tick for tick in yticks if tick > 0]

plt.xticks(fontsize=21)
plt.yticks(yticks, fontsize=21)

plt.legend(fontsize=20)
plt.ylim(0, max_y_value * 1.1)

plt.savefig("T-time_cache_comparison.pdf", format="pdf", bbox_inches="tight")

