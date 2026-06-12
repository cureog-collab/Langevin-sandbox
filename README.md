# Langevin sandbox
A real-time 2D physics simulator built in C. This project models the interactions and dynamics of Abrikosov vortices in Type-II superconductors (or any general particle system governed by the general Langevin equation). 

The primary motivation behind this project is to familiarize myself with data-oriented design programming.
## Compile and run
Use Makefile to auto-compile
```bash
make
./build/Langevin-engine [flags]
```

## Controls (Work in Progress)
* Move and zomm camera by mouse wheel and arrow keys (or ASWD).
* Press 'SPACE' to reset camera settings.
* Press 'S' to pause/ resume the simulation.
* Right-click to spawn a new particle at the cursor's location.
* 'CTRL' + right-click to annihilate a particle under the cursor.
* Left-click to spawn a new defect at the cursor's location.
* 'CTRL' + left-click to annihilate a defect under the cursor.

## Flags
* -c **(Initial particle count)**: The number of vortices generated at startup.
* -d **(Initial defect count)**: The number of defects generated at startup.
* -t **(Temperature (K))**: Pseudo-temperature (drives the Brownian motion).
* -x **(Current density Jx (x10³ A/m²))**: Pseudo-current density along the X-axis (generates a vertical Lorentz force on the vortices)
* -y **(Current density Jy (x10³ A/m²))**: Pseudo-current density along the Y-axis (generates a horizontal Lorentz force on the vortices).

## Dependencies
* **SDL2**
