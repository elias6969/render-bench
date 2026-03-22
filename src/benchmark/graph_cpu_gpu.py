import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("build/benchmark.csv", sep=';')
df = df.groupby(["renderer", "object_count"], as_index=False).mean()

renderer_map = {0: "Naive", 1: "Instanced", 2: "Batch"}

plt.figure()

for r, group in df.groupby("renderer"):
    plt.plot(group["object_count"], group["cpu_time_ms"],
             linestyle='--',
             label=f"{renderer_map.get(r)} CPU")

    plt.plot(group["object_count"], group["gpu_time_ms"],
             label=f"{renderer_map.get(r)} GPU")

plt.xlabel("Object Count")
plt.ylabel("Time (ms)")
plt.title("CPU vs GPU Time")
plt.legend()
plt.grid()
plt.tight_layout()

plt.savefig("cpu_vs_gpu.png")
plt.show()
