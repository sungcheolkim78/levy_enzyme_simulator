// enzymeWalker.cpp
//
// application using Cell and EnzymeWalker class
//
// author: sung-cheol kim @ IBM
//
// date: 20160202 version: 1.0.0
// date: 20160617 version: 1.1.0 update: change class structure
// date: 20170901 version: 1.2.0 update: modifiy how to show log
// date: 20170904 version: 1.3.0 update: unit time recording
// date: 20170907 version: 1.4.0 update: multiple cloud objects
// date: 20170911 version: 1.5.0 update: change base library
// date: 20170924 version: 1.6.0 update: fine tuning for diffusion case

#include "../base/include/Simulator.hpp"
#include "../base/include/Log.hpp"
#include "../base/include/ParameterReader.h"
#include "../base/include/CloudCell.hpp"

int main(int argc, char* argv[])
{
  // prepare parameter file

  string parname {"test.par"};
  cout << blu << "[enzymeWalker] Cell Simulator, made by Sung-Cheol Kim, version 1.6.0" << def << endl;

  if (argc == 2)
    parname = argv[1];

  ifstream f(parname.c_str());
  if ( !f.good() ) {
    cerr << "... no " << parname << endl;
    cerr << "Usage: simpleWalker [sim.par]" << endl;
    exit(0);
  }

  // read parameter file
  Log simLog{"cloud_log.txt", parname};
  ParameterReader pr{parname};

  // create cell cloud object
  Simulator s{pr, &simLog};
  s.injectClouds(pr);

  // iterates for time steps
  cout << blu << "[CellSimulator] start simulation" << def << endl;
  s.run();
}

// vim:foldmethod=syntax:foldlevel=1
