#include "../include/WalkerBase.h"
#include <fstream>

void WalkerBase::write(string savename) {
  fstream file;
  file.open(savename.c_str(), ios::out|ios::app);
  file  << age() << " " << position_.X() << " " << position_.Y()
        << " " << position_.Z() << " " << r() 
        << " " << duration() << " " << tid() << " 0" << endl;
}

void WalkerBase::setProperties(ParameterReader& pr, string cID) {
  r_ = pr.doubleRead(cID+" Particle Radius", "1")/1000.0;
  volume_ = 4.0/3.0*M_PI*r_*r_*r_;
  pid(calPID());
  cloudID(cID);
}

// vim:foldmethod=syntax:foldlevel=0
