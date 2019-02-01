// EnzymeWalker.cpp
// enzyme class implementation
//
// author: sungcheolkim @ IBM
// date: 20160202 version: 1.0.0
// date: 20160211 version: 2.1.0 update: new active site reaction
// date: 20160617 version: 2.2.0 update: usig new vec3 class
// date: 20170505 version: 2.3.0 update: add wall check condition
// date: 20170912 - rewritten with new cloud class

#include "../include/WalkerEnzyme.h"

void WalkerEnzyme::setProperties(ParameterReader& pr, string cID) {
  r_ = pr.doubleRead(cID+" Particle Radius", "1")/1000.0;  // [um]
  volume_ = 4.0/3.0*M_PI*r_*r_*r_;  // [um3]
  auto density = pr.doubleRead(cID+" Particle Density", "1"); // [g/cm3]
  mass_ = volume_*density*1e-15;   // [kg]
  pid(calPID());
  cloudID(cID);
}

// vim:foldmethod=syntax:foldlevel=0
