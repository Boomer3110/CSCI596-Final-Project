# CSCI 596 fluid simulator work split

We’ll build a 2D dam-break demo using Smoothed Particle Hydrodynamics (SPH) and measure how much a uniform grid and Apple Metal each improve simulation speed. [Team repository](https://github.com/Boomer3110/CSCI596-Final-Project)

| Deliverable | Riya | Partner (Boomer3110) |
| --- | --- | --- |
| First working version | Build the CPU solver with all-pairs neighbor search. Compute density, pressure, viscosity and gravity, advance the particles one timestep, and handle wall collisions. | Set up the Mac app and Metal particle renderer. Start with a fixed array of particle positions so the viewer can progress while Riya builds the solver. Add pause and reset. |
| Optimization | Add uniform-grid neighbor search to the CPU solver. Check that it finds the same neighbors as the all-pairs version, including particles on opposite sides of a cell boundary. | Implement the grid-based SPH solver with Metal compute kernels. Connect its output to the same viewer and add a way to switch solvers. |
| Evidence and write-up | Build the benchmark runner, plot milliseconds per step and speedup against particle count, and write the physics and results sections. | Collect Metal timings on the same Mac, record the hardware and build settings, capture the demo, and write the GPU methods and run instructions. |

## First handoff

Agree on particle positions and velocities, smoothing radius, timestep and box dimensions. Riya shares a small starting scene and its expected densities and positions after one CPU step. Boomer3110 uses that scene to check the Metal output within an agreed floating-point tolerance before running larger cases.

## Shared milestones

- **For the one-minute presentation on Nov 30 or Dec 2:** Show the preliminary repo. Riya explains the fluid problem and why neighbor search gets expensive. Boomer3110 explains the grid and Metal approach and what the team expects to measure.
- **For the final comparison:** Run all three solvers on the same Mac with identical initial conditions and particle counts. Time completed simulation steps with rendering disabled. Compare CPU all-pairs with CPU grid to measure the grid benefit, then CPU grid with Metal grid to measure the GPU benefit. Report interactive FPS separately.
- **By Dec 11:** Test the README instructions from a fresh clone, include the demo and plots, and record each person's actual contributions. Each partner submits the same repository URL individually on Brightspace.
