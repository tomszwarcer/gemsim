#include <cstdlib>
#include "Garfield/MediumMagboltz.hh"
#include <string>
#include <iostream>

using namespace Garfield;

void tableGen(double ar_percent, int rn, double val){

  std::string run_number = std::to_string(rn);

  //output directory path
  std::string dir ("/opt/ppd/scratch/szwarcer/paragem/gasgain/gastable/50/");
  std::string end (".gas");
  dir += run_number;
  dir += end;

  //Create MM object for gas and set it up
  MediumMagboltz gas;
  gas.SetComposition("cf4", 100.-ar_percent, "argon", ar_percent);
  gas.SetTemperature(293.15);
  gas.SetPressure(60.);
  gas.SetFieldGrid(val, val,1,false);
  gas.GenerateGasTable(10);
  gas.WriteGasFile(dir);
  std::cout << "Done\n";
}
