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

struct Level{
  int index;
  double energy;
  int gas_index;
  int type;
  std::string descr;
  unsigned int num_colls;
  std::string id;
  Level(int idx, double e, int g_i, int t, std::string d, unsigned int n){
    index = idx;
    energy = e;
    gas_index = g_i;
    type = t;
    descr = d;
    num_colls = n;
    id = std::to_string(g_i) + "-" +  std::to_string(t) + "-" + std::to_string(e);
  }
};

int main(){

  double ar_percent = 0.0;

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
  mapDir += "/Users/tomszwarcer/Documents/MIGDAL/UPDATE/fieldmaps/600/";

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
  const double anode_pos = -0.2 + 0.0001;
  const double max_sensor_z = 0.5;

  //this is the region electrons are tracked in
  Sensor sensor;
  sensor.AddComponent(&fm);
  sensor.SetArea(-12*pitch, -12*pitch, anode_pos-0.002, 12*pitch, 12*pitch, max_sensor_z);

  //Set up the microscopic avalanche
  AvalancheMicroscopic aval;
  aval.SetSensor(&sensor);

  //starting positions 
  double x0 = 0;
  double y0 = 0;
  const double z0 = 0.4;
  const double t0 = 0.;
  const double e0 = 1;

  unsigned int gain = 0;

  std::ofstream file;
  file.open("/Users/tomszwarcer/Documents/MIGDAL/UPDATE/test.txt");

  const unsigned int n_runs = 1;
  bool track_levels = false;
  const double track_levels_above = 13.99;

  for (int i=1; i <= n_runs; i++){
    //Do avalanche
    aval.AvalancheElectron(x0, y0, z0, t0, e0, 0., 0., 0.);

    for (const auto& electron : aval.GetElectrons()) {
      const auto& p1 = electron.path.back();
      if (p1.z <= anode_pos){
          gain++;
      }
    }
    file << gain << '\n';
    gain = 0;

    if (track_levels == true){
      unsigned int num_levels = gas.GetNumberOfLevels();

      int type;//Type of level 
      std::string descr;//Level name
      int ngas;//Gas index of the level
      double e_level;//Energy of level

      std::vector<Level> level_list;
      
      //Get levels above a certain energy
      for (int level_index=0;level_index<num_levels;++level_index){
        gas.GetLevel(level_index,ngas,type,descr,e_level);
        if (e_level > track_levels_above){
          level_list.push_back(Level(level_index,e_level,ngas,type,descr,0));
        }
      }

      //Get collisions in these levels, save to file
      for (Level level:level_list){
        level.num_colls = gas.GetNumberOfElectronCollisions(level.index);
        file << level.id << "," << level.num_colls << '\n';
      }

    }
    file << '\n';
  }
  
  file.close();
  std::cout << "ALL DONE!" << std::endl;
}