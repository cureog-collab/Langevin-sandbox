# Langevin sandbox
A real-time 2D physics simulator built in C. This project models the interactions and dynamics of Abrikosov vortices in Type-II superconductors (or any general particle system governed by the general Langevin equation). 

The primary motivation behind this project is to familiarize myself with data-oriented design programming.
## Compile and run
Use Makefile to auto-compile
```bash
make
./build/Langevin-engine [initial-number-of-particles]
```

## Controls (Work in Progress)
* Camera panning and zooming.
* Press 'SPACE' to reset camera settings.
* Press 'S' to pause/ resume the simulation.

## Flags
* -c **(Initial Particle Count)**: The number of vortices generated at startup.
* -t **(Temperature (K))**: Pseudo-temperature (drives the Brownian motion).
* -x **(Current Density Jx (x10³ A/m²))**: Pseudo-current density along the X-axis (generates a vertical Lorentz force on the vortices)
* -y **(Current Density Jy (x10³ A/m²))**: Pseudo-current density along the Y-axis (generates a horizontal Lorentz force on the vortices).

## Dependencies
* **SDL2**
