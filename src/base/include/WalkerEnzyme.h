// WalkerEnzyme.h
// subclass of Walker class for enzyme reaction
//
// author: sungcheolkim @ IBM
// date: 20160202
// date: 20160619 version: 1.1.0 update: define class
// date: 20170912 - rewritten for new class

#ifndef WALKERENZYME_H
#define WALKERENZYME_H

#include "WalkerBase.h"
#include "Vec3.hpp"
#include "ParameterReader.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
class WalkerEnzyme : public WalkerBase
{
public:
  // member functions
  void setProperties(ParameterReader& pr, string cloudID);

  // constructor
  WalkerEnzyme() : WalkerEnzyme(0.0, 0.0, 0.0) { }
  WalkerEnzyme(Vec3<double> v) : WalkerEnzyme(v.X(), v.Y(), v.Z()) { }
  WalkerEnzyme(double xi, double yi, double zi) :
    WalkerBase(xi, yi, zi),
    duration_(0),
    substrateHit_(0)
  {
    //cout << "[Walker(" << count_ << ")] is initialized at " << position_ << endl;
    ID("WalkerEnzyme");
  }
  virtual ~WalkerEnzyme() { }

  size_t substrateHit() { return substrateHit_; }
  void substrateHit(size_t hit) { substrateHit_ = hit; }
  void addSubstrateHit(size_t hit) { substrateHit_ += hit; }

  double duration() { return duration_; }
  void duration(double d) { duration_ = d; }
  void subDuration(double dt) { duration_ -= dt; }

  double mass() { return mass_; }
  void mass(double m) { mass_ = m; }

protected:

  double duration_;
  size_t substrateHit_;
  double mass_;

private:


};
/////////////////////////////////////////////////////////////////////////////
#endif
// vim: foldmethod=syntax
