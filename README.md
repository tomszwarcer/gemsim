# gemsim
Simulation of a double GEM system in Garfield++

## Info

- The `.C` files are intended to be run as ROOT macros
- `mc_micro.C` is a doubleGEM simulation that uses `AvalancheMC` in the induction and transfer regions, and `AvalancheMicroscopic` in the GEMs. It only outputs gain.
- `e_finder.C` finds the average energy of an electron at a certain point of interest. It computes this value for a single avalanche (ideally you would run many, maybe someone can update this)
- `efficiency.C` computes the total avalanche size and number of electrons collected at the anode for the double GEM system. It uses `AvalancheMicroscopic`
- `leveltrack.C` tracks the number of collisions and also number of electrons reaching anode, switching between `AvalancheMC` and `AvalancheMicroscopic`. It can track any number of levels.
- `gain.ipynb` is a python notebook that produces plots from the gain data
- `outputs/` contains data from simulations of gain, light/charge and efficiency
- `Gmsh/` contains the Gmsh model file and the `.sif` files needed to run Elmer 
- `condor/` contains files needed to run this code in parallel
- `data_analysis/` contains data analysis python notebooks
- `gastable/` contains files needed to generate gas files for use in `AvalancheMC`
- `gastable/tableArgGen.py` generates input arguments for running `tableGen.C` in parallel
- `gastable/tableGen.C` generates a gas file for a single electric field
- `gastable/merge.C` merges the different electric field gas files into a single file for a given Argon percentage


## What you need

- Gas table from Magboltz
- Double GEM mesh file (you can get this from the .geo file)
- ElmerSolver file (you can get this from the .msh and the .sif)
- Generate empty `batch.log`, `batch.out` and `batch.err` files

## How to use

- You may need to source setupGarfield or do other steps required to run Garfield and ROOT properly
- You may need to use `chmod` on `run_root.sh` if using condor

## ID system

I have developed a system for assigning IDs to Magboltz levels. This works as follows.

- **1st digit:** the gas index. 0 for CF4, 1 for Ar
- **2nd digit:** the type. See table 3.2 in the Garfield++ user guide
- **3rd-last digit:** the energy. There may be floating point errors here so some rounding may be required in post.
 
