import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("build/benchmark.csv", sep=';')
df = df.groupby(["renderer", "object_count"], as_index=False).mean()

renderer_map = {0: "Naive", 1: "Instanced", 2: "Batch"}

plt.figure()

for r, group in df.groupby("renderer"):
    plt.plot(group["object_count"], group["frame_time_ms"],
             label=renderer_map.get(r, f"Renderer {r}"))

plt.xlabel("Object Count")
plt.ylabel("Frame Time (ms)")
plt.title("Frame Time vs Object Count")
plt.legend()
plt.grid()
plt.tight_layout()

plt.savefig("frametime_vs_objects.png")
plt.show()
