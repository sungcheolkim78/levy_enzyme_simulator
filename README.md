# levy_enzyme_simulator
Enzyme simulator based on random walker with Levy distribution

## Install & Compile

```
> cd src/enzymeWalker
> mkdir build
> cd build
> cmake ..
> make -j4
> make install
```

## Usage

prepare simulation parameter file (sim.par or test.par)

### test.par

```
# Simulator

dt: 0.0007
iteration: 2000
species Number: 2
save True
save Cycle: 5
info Cycle: 100
save Step: False
save Count: True
show Progress: True
debug: True
species Name: (Enzyme, Substrate)

# Enzyme

Enzyme Surface Shape: Cell
Enzyme Surface Type: vol
#Enzyme Box Dimensions: (1, 2, 3)
Enzyme Cell Length: 6
Enzyme Cell Radius: 1
#Enzyme Sphere Radius: 1
Enzyme Band Position: 0.0
Enzyme Band Width: 0.2
Enzyme Ring Depth: 0.1

Enzyme Walker Type: Enzyme
Enzyme Concentration: 0.25
#Enzyme Particle Number: 1
#Enzyme Diffusion Constant: 1.0
Enzyme Particle Radius[nm]: 2.5
Enzyme Particle Density[g/cm3]: 1.0
Enzyme Temperature: 300
Enzyme Viscosity: 0.001
Enzyme Alpha: 2.0

Enzyme Injection Method: vol
#Enzyme Initial Position: (0.2, 0.2, 0.2)

Enzyme Substrate On: True
Enzyme Substrate Constant: True
Enzyme Substrate Name: Substrate
Enzyme Sight Distance[nm]: 7.5
Enzyme Reaction On: True
Enzyme Focus Concentration[uM]: 0.0
Enzyme Write Count: True
Enzyme Km: 8.9
Enzyme Kcat: 6.3

Enzyme Debug: False

# Substrate

Substrate Surface Shape: Cell
Substrate Surface Type: vol
#Substrate Box Dimensions: (1, 2, 3)
Substrate Cell Length: 6
Substrate Cell Radius: 1
#Substrate Sphere Radius: 1
Substrate Ring Depth: 0.1
Substrate Band Position: 0.0
Substrate Band Width: 0.2

Substrate Walker Type: Base
Substrate Concentration[uM]: 0.25
#Substrate Particle Number: 100
Substrate Particle Density[g/cm3]: 1.0
Substrate Particle Radius[nm]: 5.0
Substrate Particle Density[g/cm3]: 1.0
#Substrate Diffusion Constant: 0.0
Substrate Temperature: 300
Substrate Viscosity: 0.001
Substrate Alpha: 2.0

Substrate Injection Method: Random
#Substrate Initial Position: (-0.2, -0.2, -0.2)

Substrate Substrate On: False

Substrate Debug: False
```

## Visualization

