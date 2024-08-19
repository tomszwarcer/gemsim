//TODO: ion mobility
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <random>

#include "Garfield/ComponentElmer.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/AvalancheMC.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/Medium.hh"

#include <string>
#include <vector>
using namespace Garfield;

std::vector<double> colls_list;
//track CF4 neutral diss from threshold to ionisation energy
std::vector<double> tracked_energies_cf4 = {12.5,15.63};
//track ar excitaion from threshold as given by Seravalli PhD thesis to ionisation energy of CF4
std::vector<double> tracked_energies_ar = {12.907,15.66};
std::vector<int> levels_in_range;

struct passOn{
  double x,y,z,t,e,dx,dy,dz;
  passOn(double x0,double y0,double z0,double t0,double e0,double dx0,double dy0,double dz0){
    x = x0;
    y = y0;
    z = z0;
    t = t0;
    e = e0;
    dx = dx0;
    dy = dy0;
    dz = dz0;
  }
};

void userHandle(double x, double y, double z, double t,
int type, int level, Medium* m,
double e0, double e1,
double dx0, double dy0, double dz0,
double dx1, double dy1, double dz1){
  for (int j=0;j<levels_in_range.size();++j){
    if (level == levels_in_range[j]){
      ++colls_list[j];
    }
  }
}

void leveltrack(double ar_percent, int rn){

  std::string run_number = std::to_string(rn);

  //Create MM object for gas and set it up
  MediumMagboltz gas;
  gas.SetComposition("cf4", 100.-ar_percent, "argon", ar_percent);
  gas.SetTemperature(293.15);
  gas.SetPressure(60.);
  gas.Initialise(); 
  gas.LoadGasFile("/opt/ppd/scratch/szwarcer/paragem/gasgain/gastable/table30.gas");

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

   //sensor positions
  const double tgemt = 0.32;
  const double tgemb = 0.24;
  const double transb = 0.06;
  const double bgemb = -0.01;
  const double indb = -0.201;

  //this is the region electrons are tracked in
  Sensor tgem;
  tgem.AddComponent(&fm);
  tgem.SetArea(-12*pitch, -12*pitch, tgemb, 12*pitch, 12*pitch, tgemt);

  Sensor trans;
  trans.AddComponent(&fm);
  trans.SetArea(-12*pitch, -12*pitch, transb, 12*pitch, 12*pitch, tgemb+0.01);

  Sensor bgem;
  bgem.AddComponent(&fm);
  bgem.SetArea(-12*pitch, -12*pitch, bgemb, 12*pitch, 12*pitch, transb+0.01);

  Sensor ind;
  ind.AddComponent(&fm);
  ind.SetArea(-12*pitch, -12*pitch, indb-0.01, 12*pitch, 12*pitch, bgemb+0.01);

  //Set up the microscopic avalanche
  AvalancheMicroscopic av_tgem;
  av_tgem.SetSensor(&tgem);

  AvalancheMC av_trans;
  av_trans.SetSensor(&trans);

  AvalancheMicroscopic av_bgem;
  av_bgem.SetSensor(&bgem);
  av_bgem.SetUserHandleCollision(userHandle);

  AvalancheMC av_ind;
  av_ind.SetSensor(&ind);

  //set up transfer between different regions
  //top gem > transfer
  std::vector<passOn> tgemtrans;
  //trasfer > bottom gem
  std::vector<passOn> transbgem;
  //bottom gem > induction
  std::vector<passOn> bgemind;

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
  //get list of levels in range
  for (int level_index=0;level_index<num_levels;++level_index){
    gas.GetLevel(level_index,ngas,type,descr,e);
    if (e <= tracked_energies_cf4[1] + 0.0001 and e >= tracked_energies_cf4[0] - 0.0001){
      levels_in_range.push_back(level_index);
      colls_list.push_back(0);
    }
    else if (e <= tracked_energies_ar[1] + 0.0001 and e >= tracked_energies_ar[0] - 0.0001){
      levels_in_range.push_back(level_index);
      colls_list.push_back(0);
    }
  }
  std::cout << "Tracking levels ";
  for (int k=0;k<levels_in_range.size();++k){
    std::cout << levels_in_range[k] << ", "; 
  }

  double xs,ys,zs,xe,ye,ze,ts,te;
  int status;

  //average energy an electron has when it is at the top of the second GEM (taken from previous simulations)
  double e_trans = 4.57;


  //generate avalanche
  av_tgem.AvalancheElectron(x0,y0,z0,t0,e0,0.,0.,0.);
  for (const auto& electron : av_tgem.GetElectrons()){
    //pass on all endpoints that made it to the next region
    if (electron.path.back().z <= tgemb + 0.001 and electron.path.back().z >= tgemb - 0.001){
      passOn new_electron_trans(electron.path.back().x,electron.path.back().y,electron.path.back().z,electron.path.back().t,electron.path.back().energy,electron.path.back().kx,electron.path.back().ky,electron.path.back().kz);
      tgemtrans.push_back(new_electron_trans);
    }
  }

  std::cout << "transferring "<<tgemtrans.size() << " to transfer region\n";
  for (int j=0;j<tgemtrans.size();++j){
    //avalanche the passed-on endpoints
    av_trans.AvalancheElectron(tgemtrans[j].x,tgemtrans[j].y,tgemtrans[j].z,tgemtrans[j].t,false);
    //for each drift line in the most recent avalanche
    for (int k=0;k<av_trans.GetNumberOfElectronEndpoints();++k){
      //get the start and endpoints of the drift line
      av_trans.GetElectronEndpoint(k,xs,ys,zs,ts,xe,ye,ze,te,status);
      //pass on if it made it to the next region
      if (ze<=transb+0.001 and ze >= transb-0.001){
        passOn new_electron_bgem(xe,ye,ze,te,e_trans,0.,0.,0.);
        transbgem.push_back(new_electron_bgem);
      }
    }
  }

  std::cout << "transferring "<<transbgem.size() << " to bottom GEM\n";
  //avalanche all the passed-on electrons
  for (passOn &ielectron:transbgem){
    av_bgem.AvalancheElectron(ielectron.x,ielectron.y,ielectron.z,ielectron.t,ielectron.e,ielectron.dx,ielectron.dy,ielectron.dz);
    for (const auto& electron : av_bgem.GetElectrons()){
      //pass on the ones that made it to the next region
      if (electron.path.back().z <= bgemb + 0.001 and electron.path.back().z >= bgemb - 0.001){
        passOn new_electron(electron.path.back().x,electron.path.back().y,electron.path.back().z,electron.path.back().t,electron.path.back().energy,electron.path.back().kx,electron.path.back().ky,electron.path.back().kz);
        bgemind.push_back(new_electron);
      }
    }
  }
  std::cout << "transferring "<<bgemind.size() << " to induction region\n";
  //avalanche the ones that made it
  for (passOn &electron:bgemind){
    av_ind.AvalancheElectron(electron.x,electron.y,electron.z,electron.t,false);
    //for each driftline
    for (int k=0;k<av_ind.GetNumberOfElectronEndpoints();++k){
      //check if the endpoint is within a tolerance of the anode
      av_ind.GetElectronEndpoint(k,xs,ys,zs,ts,xe,ye,ze,te,status);
      if (ze <= anode_pos + 0.001){
        anode_counter += 1;
      }
    }
  }

  std::ofstream file;
  file.open("/opt/ppd/scratch/szwarcer/paragem/charge_light/cl_600_30/" + run_number + ".csv");
  file << anode_counter << std::endl;
  for (int i=0;i<levels_in_range.size();++i){
    file << colls_list[i] << std::endl;
  }
  file.close();
  std::cout << "ALL DONE!" << std::endl;
}
