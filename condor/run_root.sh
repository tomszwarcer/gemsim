#!/bin/bash
INPUT_arpercent=$1
INPUT_runnum=$2

# change to your directory path
cd /opt/ppd/scratch/szwarcer/paragem/charge_light

# required to run ROOT/Garfield (elmer not required)
source /cvmfs/sft.cern.ch/lcg/views/LCG_104c/x86_64-el9-gcc13-opt/setup.sh
export PATH=/opt/ppd/darkmatter/elmer/install/bin/:$PATH
export GARFIELD_HOME=/opt/ppd/darkmatter/garfield-new
source /opt/ppd/darkmatter/garfield-new/build/setupGarfield.sh

# change the file being run to whichever one you need
root.exe -b -q "leveltrack.C($INPUT_arpercent,$INPUT_runnum)"
