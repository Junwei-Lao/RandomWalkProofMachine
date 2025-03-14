# Random Walk State Machine
## Description
This project implements a **random walk state machine** designed for mathematical analysis, specifically testing how the behavior of a random walk changes within different boundary shapes.

The machine stops based on one of two conditions:

1. The **convex hull area** of the path reaches **half of the boundary area** (files in the ```area``` folder).
2. The machine **traverses half of the points** inside the boundary (files in the ```unique_points``` folder).
## Functionality
- The scripts in the ```area``` folder analyze the number of steps required for the convex hull area of the path to reach half of the shape’s total area.

- The scripts in the ```unique_points``` folder examine the number of steps needed for the machine to cover half of the unique points within the boundary.

- Files with names ending in "**multiple**" execute multiple random walk simulations to analyze the **distribution of total steps** before stopping.

- Files **without** "multiple" run a **single** random walk to visualize how the machine moves within a specific shape.

This project provides a foundation for further mathematical proofs and experimental studies on random walk behavior in constrained environments.



