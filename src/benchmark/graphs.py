import pandas as pd
import matplotlib.pyplot as plt

# Load
df = pd.read_csv("build/benchmark.csv", sep=';')

# Clean / group data
df_clean = df.groupby(["renderer", "object_count"], as_index=False).mean()

# Optional: sort
df_clean = df_clean.sort_values("object_count")

# Renderer names
renderer_map = {
    0: "Naive",
    1: "Instanced",
    2: "Batch"
}

# Graph 1: FPS vs Object Count
plt.figure()

for renderer, group in df_clean.groupby("renderer"):
    plt.plot(group["object_count"], group["fps"],
             marker='o',
             label=renderer_map.get(renderer, f"Renderer {renderer}"))

plt.xlabel("Object Count")
plt.ylabel("FPS")
plt.title("FPS vs Object Count")
plt.legend()
plt.grid()
plt.tight_layout()
plt.show()

# Graph 2: Frame Time vs Object Count
plt.figure()

for renderer, group in df_clean.groupby("renderer"):
    plt.plot(group["object_count"], group["frame_time_ms"],
             marker='o',
             label=renderer_map.get(renderer, f"Renderer {renderer}"))

plt.xlabel("Object Count")
plt.ylabel("Frame Time (ms)")
plt.title("Frame Time vs Object Count")
plt.legend()
plt.grid()
plt.tight_layout()
plt.show()

# Graph 3: CPU vs GPU 
plt.figure()

for renderer, group in df_clean.groupby("renderer"):
    plt.plot(group["object_count"], group["cpu_time_ms"],
             linestyle='--',
             label=f"{renderer_map.get(renderer)} CPU")

    plt.plot(group["object_count"], group["gpu_time_ms"],
             label=f"{renderer_map.get(renderer)} GPU")

plt.xlabel("Object Count")
plt.ylabel("Time (ms)")
plt.title("CPU vs GPU Time")
plt.legend()
plt.grid()
plt.tight_layout()
plt.show()
