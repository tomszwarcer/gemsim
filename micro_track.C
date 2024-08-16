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

std::vector<double> colls_list(2);
std::vector<double> tracked_energies = {12.5,12.907};
std::vector<int> tracked_levels(2);


void userHandle(double x, double y, double z, double t,
int type, int level, Medium* m,
double e0, double e1,
double dx0, double dy0, double dz0,
double dx1, double dy1, double dz1){
  if (level == tracked_levels[0]){
    ++colls_list[0];
  }
  else if (level == tracked_levels[1] and level != 0){
    ++colls_list[1];
  }
}

void micro_track(double ar_percent, int rn){

  std::string run_number = std::to_string(rn);

  //Create MM object for gas and set it up
  MediumMagboltz gas;
  gas.SetComposition("cf4", 100.-ar_percent, "argon", ar_percent);
  gas.SetTemperature(293.15);
  gas.SetPressure(50.);
  gas.Initialise();

  //load ion mobility data
  //gas.LoadIonMobility("/path/to/file");

  //Setup handover from Elmer
  ComponentElmer fm;

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

  //define consts
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
  aval.SetUserHandleCollision(userHandle);
  unsigned int anode_counter = 0;

  //starting positions 
  double x0 = 0;
  double y0 = 0;
  const double z0 = 0.32;
  const double t0 = 0.;
  const double e0 = 0.1;

  int ngas;
  int type;
  std::string descr;
  double e;
  unsigned int num_levels = gas.GetNumberOfLevels();    
  for (int level_index=0;level_index<num_levels;++level_index){
    gas.GetLevel(level_index,ngas,type,descr,e);
    if (e <= tracked_energies[0] + 0.0001 and e >= tracked_energies[0] - 0.0001){
      tracked_levels[0] = level_index;
    }
    else if (e <= tracked_energies[1] + 0.0001 and e >= tracked_energies[1] - 0.0001){
      tracked_levels[1] = level_index;
    }
  }
  std::cout << "Tracking levels " <<  tracked_levels[0] << ", " << tracked_levels[1] << std::endl;;


    
  //generate avalanche
  aval.AvalancheElectron(x0, y0, z0, t0, e0, 0., 0., 0.);
  for (const auto& electron : aval.GetElectrons()){
    if (electron.path.back().z <= anode_pos + 0.001){
      anode_counter += 1;
    }
  }

  std::ofstream file;
  file.open("/opt/ppd/scratch/szwarcer/paragem/charge_light/cl_600_0/" + run_number + ".csv");
  file << anode_counter << std::endl;
  file << colls_list[0] << std::endl;
  file << colls_list[1] << std::endl;
  file.close();
  std::cout << "ALL DONE!" << std::endl;
}