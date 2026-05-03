# render-bench

A small C++23 benchmark application that compares three rendering strategies in
modern OpenGL on the same scene of textured cubes:

- **Naive** – one draw call per object, model matrix uploaded as a uniform
  inside a tight loop (`glDrawArrays` per cube).
- **Instanced** – one `glDrawArraysInstanced` call, with a per-instance model
  matrix supplied through a vertex attribute buffer
  (`glVertexAttribDivisor(..., 1)`).
- **Batch** – all object vertices pre-transformed into world space at startup
  and packed into a single static vertex buffer, drawn with a single
  `glDrawArrays`.

The application records frames per second, frame time, CPU rendering time
(`std::chrono::high_resolution_clock` around the renderer's `Render` call) and
GPU rendering time (`glBeginQuery`/`glEndQuery` with `GL_TIME_ELAPSED` around
the same call) at 120 object-count steps from 1 to 50000, for each of the three
renderers, with a 0.5 s warm-up and a 1 s averaging window per step. The
results are written to `build/benchmark.csv`.

The benchmark was developed as part of a degree project. The accompanying
report and figures live in
[../../school/exampapers](../../school/exampapers).

## Requirements

- A C++23-capable compiler (clang or gcc).
- CMake 3.10 or newer.
- An OpenGL 4.5 driver and an X / Wayland session (or equivalent) to create a
  window.
- Assimp (`libassimp-dev` on Debian, `assimp` on Arch).
- GLFW is found through `find_package(glfw3)` if available, otherwise the
  bundled submodule under `external/glfw` is built automatically.

GLAD, ImGui (docking branch) and a CSV writer are vendored under `external/`
and require no separate installation.

## Build

```bash
git clone https://github.com/elias6969/render-bench.git
cd render-bench
cmake -S . -B build
cmake --build build -j
```

The resulting executable is `build/Engine`.

## Run

```bash
./build/Engine
```

Once the window is open the ImGui panel labelled **Benchmark** exposes the
controls:

- **Object Count** – manually scrub between 1 and 50000 cubes.
- **Renderer** – switch between Naive, Instanced and Batch at runtime.
- **VSync** – toggle vertical synchronisation. Disabled by default for
  benchmarking; the application also forces `glfwSwapInterval(0)` on
  startup.

Live frame statistics, including CPU/GPU rendering times, 1% and 0.1% lows
and a frame stability percentage, are shown in the same panel.

## Run the automated benchmark

Click **Run Benchmark**. The benchmark sweeps object count linearly through
120 steps from 1 to 50000 for each renderer in turn, with a 0.5 s warm-up and
a 1 s measurement window per step. When the third renderer finishes, the
console prints `Benchmark complete!` and `build/benchmark.csv` is written
out.

The CSV contains one row per step with the columns:

```
object_count;fps;frame_time_ms;cpu_time_ms;gpu_time_ms;renderer;draw_calls
```

`renderer` is `0` for Naive, `1` for Instanced and `2` for Batch. `draw_calls`
equals `object_count` for the naive renderer and `1` for the other two.

## Generate the figures

A small plotting script using `pandas` and `matplotlib` reproduces the three
figures used in the thesis (FPS, frame time and CPU vs GPU time):

```bash
python src/benchmark/graphs.py
```

The script reads `build/benchmark.csv` and opens three matplotlib windows.

## Project layout

```
include/        public headers
src/            engine and renderer sources
src/renderers/  NaiveRenderer / InstancedRenderer / BatchRenderer
src/tools/      timing, CSV export, helpers
src/benchmark/  python plotting script
external/       vendored third-party libraries (GLAD, ImGui, GLFW, GLM, CSVWriter)
```

## Reference hardware

The CSV file shipped with the report was produced on:

- Intel Core i5-8400 (six cores, 2.8 GHz)
- NVIDIA GeForce GTX 1050 (GP107)
- 16 GB RAM
- Arch Linux, kernel 6.19.11
- 1920 x 1080 display, vertical synchronisation disabled

Source revision used for the report: commit `c2f1252`.
