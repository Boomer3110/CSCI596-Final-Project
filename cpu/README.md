# CPU fluid solver

Both C++ solvers run the same 2D dam break. The baseline checks every particle for neighbors. The grid version searches the surrounding cells.

Run from the repo root:

```sh
clang++ -std=c++17 -O3 cpu/main.cpp -o /tmp/sph
/tmp/sph check
/tmp/sph benchmark
```

For the Metal comparison, start the 96 particles at the coordinates in [first-step.csv](first-step.csv) with zero velocity. Compare density before the update and position and velocity after one step. The constants and update order are in [sph.hpp](sph.hpp). Run `/tmp/sph reference` to print the CPU values again.

On an Apple M4 with 3,456 particles, the grid took 0.88 ms per step and the baseline took 37.62 ms. [Timings](benchmark.csv) · [Plot](benchmark.svg)
