//TODO: ion mobility
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <random>

#include "Garfield/ComponentElmer.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/Medium.hh"

#include <string>
#include <vector>

using namespace Garfield;

void micro(double ar_percent, int rn){

  std::string run_number = std::to_string(rn);

  //Create MM object for gas and set it up
  MediumMagboltz gas;
  gas.SetComposition("cf4", 100.-ar_percent, "argon", ar_percent);
  gas.SetTemperature(293.15);
  gas.SetPressure(60.);
  gas.Initialise();

  //Setup handover from Elmer
  ComponentElmer fm;

  //configure the field map. Make sure to set it to the one with the correct GEM dV
  std::string mapDir = "";
  //The path must end in a "/"
  mapDir += "/opt/ppd/scratch/szwarcer/paragem/Gmsh/dGem600/";

  fm.Initialise(mapDir + "mesh.header",
                mapDir + "mesh.elements",
                mapDir + "mesh.nodes",
                mapDir + "dielectrics.dat",
                mapDir + "mesh.result",
                "mm");
  fm.EnableTetrahedralTreeForElementSearch(true);
  fm.EnablePeriodicityX();
  fm.EnablePeriodicityY();
  fm.SetGas(&gas);
  fm.SetMedium(0,&gas);

  //define geometry consts
  const double pitch = 0.0280;
  const double hole_radius = 0.017/2.;
  const double height = sqrt(3)*pitch;
  const double anode_pos = -0.2;
  const double max_sensor_z = 0.34;

  //this is the region electrons are tracked in
  Sensor sensor;
  sensor.AddComponent(&fm);
  sensor.SetArea(-12*pitch, -12*pitch, anode_pos, 12*pitch, 12*pitch, max_sensor_z);

  //Set up the microscopic avalanche
  AvalancheMicroscopic aval;
  aval.SetSensor(&sensor);
  unsigned int anode_counter = 0;


  //starting positions 
  double x0 = 0;
  double y0 = 0;
  const double z0 = 0.32;
  const double t0 = 0.;
  const double e0 = 0.1;

  
  //Do avalanche
  aval.AvalancheElectron(x0, y0, z0, t0, e0, 0., 0., 0.);
  for (const auto& electron : aval.GetElectrons()){
    if (electron.path.back().z <= anode_pos + 0.01){
      anode_counter += 1;
    }
  }
  
  std::ofstream file;
  file.open("/opt/ppd/scratch/szwarcer/paragem/gasgain/output615_0/" + run_number + ".csv");
  file << anode_counter << std::endl;
  file.close();
  std::cout << "ALL DONE!" << std::endl;
}
