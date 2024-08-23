//Change 50 to whatever argon percentage you are using
#include <cstdlib>
#include "Garfield/MediumMagboltz.hh"
#include <string>
#include <iostream>

using namespace Garfield;

void merge(double ar_percent){

  //Create MM object for gas and set it up
  MediumMagboltz gas;
  gas.SetComposition("cf4", 100.-ar_percent, "argon", ar_percent);
  gas.SetTemperature(293.15);
  gas.SetPressure(60.);
  gas.LoadGasFile("/opt/ppd/scratch/szwarcer/paragem/gasgain/gastable/50/1.gas");
  gas.WriteGasFile("/opt/ppd/scratch/szwarcer/paragem/gasgain/gastable/50/table50.gas");
  for (int i=2;i<=100;++i){
    gas.LoadGasFile("/opt/ppd/scratch/szwarcer/paragem/gasgain/gastable/50/"+std::to_string(i)+".gas");
    gas.MergeGasFile("/opt/ppd/scratch/szwarcer/paragem/gasgain/gastable/50/table50.gas",true);;
    gas.WriteGasFile("/opt/ppd/scratch/szwarcer/paragem/gasgain/gastable/50/table50.gas");
    std::cout << "merged "<< i << std::endl;
  }

  std::cout << "Done\n";
}
