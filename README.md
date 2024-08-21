# gemsim
Simulation of a double GEM system in Garfield++

## Info

- The `.C` files are intended to be run as ROOT macros
- `mc_micro.C` is a doubleGEM simulation that uses `AvalancheMC` in the induction and transfer regions, and `AvalancheMicroscopic` in the GEMs. It only outputs gain.
- `e_finder.C` finds the average energy of an electron at a certain point of interest.
- `efficiency.C` computes the total avalanche size and number of electrons collected at the anode for the double GEM system. It uses `AvalancheMicroscopic`
- `leveltrack.C` tracks the number of collisions and also number of electrons reaching anode, switching between `AvalancheMC` and `AvalancheMicroscopic`. It can track any number of levels.
- `gain.ipynb` is a python notebook that produces plots from the gain data
- `outputs/` contains data from simulations of gain, light/charge and efficiency
- `Gmsh/` contains the Gmsh model file and the `.sif` files needed to run Elmer 
- `condor/` contains files needed to run this code in parallel
- `data_analysis` contains data analysis python notebooks


## What you need

- Gas table from Magboltz
- Double GEM mesh file (you can get this from the .geo file)
- ElmerSolver file (you can get this from the .msh and the .sif)
- Generate empty `batch.log`, `batch.out` and `batch.err` files

## How to use

- You may need to source setupGarfield or do other steps required to run Garfield and ROOT properly
- You may need to use `chmod` on `run_root.sh` if using condor

 
