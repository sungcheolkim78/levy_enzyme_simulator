// Cloud.h
// collection of walkers
//
// author: sungcheolkim @ IBM
// date: 20160202
// date: 2017/09/06 - generalize for multiple clouds
// date: 2017/09/29 - virtual cloud for particle particle interaction
//

#ifndef CLOUDBASE_H
#define CLOUDBASE_H

#include <gsl/gsl_const_num.h>
#include <gsl/gsl_const_mksa.h>
#include "Vec3.hpp"
#include "Cloud.hpp"
#include "ParameterReader.h"
#include "Surfaces.hpp"
#include "SurfacesSphere.hpp"
#include "SurfacesBox.hpp"
#include "SurfacesCell.hpp"
#include "Walker.h"
#include "WalkerBase.h"
#include "WalkerEnzyme.h"
#include <set>

using namespace std;

class CloudBase: public Cloud {

public:
  // member functions
  void setProperties(ParameterReader& pr);
  void injectWalkers(ParameterReader& pr);
  virtual void moveWalker(double dt);
  virtual void info(Log* log_);
  virtual void setSubstrateCloud(Cloud* sc);

  // constructor
  CloudBase(ParameterReader& pr, string cID) :
    viscosity_(0.001),
    temperature_(300.0) {
    cout <<"[Cloud(" << cID << ")] is initialized." << endl;

    cloudID(cID);
    string tmp = pr.simfilename();
    if(tmp.find(".par") != string::npos)
      savefilename(tmp.substr(0, tmp.find(".par")) + "_" + cID + ".pt");
    else if(tmp.find(".txt") != string::npos)
      savefilename(tmp.substr(0, tmp.find(".txt")) + "_" + cID + ".pt");
    else 
      savefilename(tmp + "_" + cID + ".pt");
    alpha(pr.doubleRead(cID+" Alpha", "2.0"));
    surfaceShape(pr.stringRead(cID+" Surface Shape", "Sphere"));
    walkerType(pr.stringRead(cID+" Walker Type", "Base"));
    debug(pr.boolRead(cID+" Debug", "False"));
    dt_ = pr.doubleRead("dt", "0.0001");

    // initialize pid list
    set<size_t> empty;
    for(auto i=0; i < 16; i++) pidList_.push_back(empty);

    writeHeader(savefilename());
    setProperties(pr);
  }
  virtual ~CloudBase() { };

  // inline functions
  inline double concentration() { return concentration_; }
  inline void concentration(double c) { concentration_ = c; }
  inline double r() { return r_; }
  inline void r(double radius) { r_ = radius; }
  inline double temperature() { return temperature_; }
  inline void temperature(double t) { temperature_ = t; }
  inline double viscosity() { return viscosity_; }
  inline void viscosity(double v) { viscosity_ = v; }
  inline bool debug() { return debug_; }
  inline void debug(bool t) { debug_ = t; }

protected:
  // particle related information
  double concentration_;
  double viscosity_;
  double temperature_;
  double r_;
  bool debug_;

private:
  double meanVel_;

};

void CloudBase::setProperties(ParameterReader& pr) {
  // find surface type
  if (surfaceShape().find("Sphere") != string::npos) {
    sf_ = new SurfacesSphere{pr, cloudID()};
  } else if (surfaceShape().find("Box") != string::npos) {
    sf_ = new SurfacesBox{pr, cloudID()};
  } else if (surfaceShape().find("Cell") != string::npos) {
    sf_ = new SurfacesCell{pr, cloudID()};
  } else {
    cerr << "... not know surface shape type: " << surfaceShape() << " from (Sphere, Box, Cell)" << endl;
    exit(1);
  }

  if (pr.checkName(cloudID()+" Diffusion Constant")) {
    D_ = pr.doubleRead(cloudID()+" Diffusion Constant", "1.0");
  } else {
    viscosity_ = pr.doubleRead(cloudID()+" Viscosity", "0.001");
    temperature_ = pr.doubleRead(cloudID()+" Temperature", "300");
    r_ = pr.doubleRead(cloudID()+" Particle Radius", "1")/1000.0;
    D_ = GSL_CONST_MKSA_BOLTZMANN*temperature_*1e18/(6.0*M_PI*viscosity_*r_);   // [um^2/s]
    cout << "... D: " << D_ << " [um2/s]" << endl;
  }
}

void CloudBase::setSubstrateCloud(Cloud* sc) {
  cerr << "... CloudBase: cannot have " << sc->cloudID() << " cloud as substrate." << endl;
}

void CloudBase::injectWalkers(ParameterReader& pr) {
  Vec3<double> p0;
  bool rflag = false;
  size_t initialCount_;

  // calculate particle number
  if (pr.checkName(cloudID_+" Particle Number")) {
    initialCount_ = pr.intRead(cloudID_+" Particle Number", "1");

    concentration((double)initialCount_/(sf_->typeVolume()*GSL_CONST_NUM_AVOGADRO*1e-21));
    cout << "... concentration: " << concentration() << " [uM]" << endl;
  } else if (pr.checkName(cloudID_+" Concentration")) {
    double cell_concentration = pr.doubleRead(cloudID_+" Concentration", "1.0");

    initialCount_ = (int)(cell_concentration*sf_->volume()*GSL_CONST_NUM_AVOGADRO*1e-21);
    cout << "... particle number: " << initialCount_ << endl;
    concentration((double)initialCount_/(sf_->typeVolume()*GSL_CONST_NUM_AVOGADRO*1e-21));
    cout << "... local concentration: " << concentration() << " [uM]" << endl;
  } else {
    initialCount_ = pr.intRead(cloudID_+" Particle Number", "100");

    concentration((double)initialCount_/(sf_->typeVolume()*GSL_CONST_NUM_AVOGADRO*1e-21));
    cout << "... concentration: " << concentration() << " [uM]" << endl;
  }

  cout << "... inject " << initialCount_ << " (" << walkerType() << " Type) Walker in Cloud(" << cloudID() << ")" << endl;

  // calculate initial position
  string res = pr.stringRead(cloudID_+" Injection Method", "Random");
  SurfaceTypeClass sc = sf_->stype();
  if (res.find("Random") != string::npos) {
    rflag = true;
  } else if (res.find("Fixed") != string::npos) {
    // set fixed position
    p0 = pr.vec3Read(cloudID_+" Initial Position", "(0, 0, 0)");
    if (!sf_->isInside(p0)) {
      cerr << "... " << p0 << " is not inside." << endl;
      exit(1);
    }
  } else if (res.find("vol") !=string::npos) {
    rflag = true; sc = SurfaceTypeClass::volume;
  } else if (res.find("sur") !=string::npos) {
    rflag = true; sc = SurfaceTypeClass::surface;
  } else if (res.find("disk") !=string::npos) {
    rflag = true; sc = SurfaceTypeClass::disk;
  } else if (res.find("ring") !=string::npos) {
    rflag = true; sc = SurfaceTypeClass::ring;
  } else {
    cerr << "... Initial Position: [Random|Fixed|vol|sur|disk|ring] " << res << endl;
    exit(1);
  }

  // create walker and set properties
  Walker* w;
  for(size_t i=0; i < initialCount_; i++) {
    // calculate random position
    if (rflag)  p0 = sf_->calRandomPosition(rs_, sc);

    // check walker type : Base, Enzyme
    if (walkerType().find("Base") != string::npos) {
      w = new WalkerBase{p0};
    } else if (walkerType().find("Enzyme") != string::npos) {
      w = new WalkerEnzyme{p0};
    } else {
      cerr << "... not know walker type " << walkerType() << " from (Base, Enzyme)" << endl;
      exit(1);
    }
    w->tid(i);
    auto v1 = sf_->maxDimension();
    auto v2 = sf_->minDimension();
    //cout << "... pid criteria px1: " << v2.X()/2.0 << " px3: " << v1.X()/2.0 << endl;
    w->setPIDRange(v2.X()/2.0, 0.0, v1.X()/2.0, 0.0, 0.0);
    w->setProperties(pr, cloudID());
    addWalker(w);
  }

  // show pid list and number in it
  cout << "... pid list ";
  for(size_t i=0; i < pidList_.size(); ++i)
    cout << i << "(" << pidList_[i].size() << ") ";
  cout << endl;
}

void CloudBase::moveWalker(double dt) {
  for (auto w : wlist_) {
    // time(age) shift
    w->addAge(dt);

    // fixed position clouds
    if (D() == 0.0) continue;

    Vec3<double> dr;
    dr = getStep(dt);

    // check wall hit
    if (!sf_->isInside(w->position()+dr)) {
      double tt = sf_->getTimeForSurface(w->position(), dr);
      dr = sf_->calNewStep(w->position(), dr, tt);
      w->addWallHit(1);
      //cout << "... hit wall at " << wlist_[i].position()+dr << endl;
    }

    // move walker
    auto p_pid = w->pid();
    w->step(dr);
    w->addAge(dt);
    auto n_pid = w->pid();
    if (p_pid != n_pid) {
      pidList_[p_pid].erase(w->tid());
      pidList_[n_pid].insert(w->tid());
    }
  }
}

void CloudBase::info(Log* log_) {
  // collect informations from walkers
  size_t totalWallHit{0};
  double age = (*this)[0]->age();

  cout << "Time: " << age << " [s]" << endl;

  for (auto w : wlist_) { totalWallHit += w->wallHit(); }
  cout << "Wall Hit: " << totalWallHit << endl;
  auto wpressure = 1e14*2.0*(*this)[0]->mass()*meanVel_*(double)totalWallHit/(sf_->surfaceArea()*age);
  cout << "Wall Hit Pressure: " << wpressure << " [mbar]" << endl;
}
#endif

// vim: foldmethod=syntax
