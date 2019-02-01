// Walker.h
//
// author: sungcheolkim @ IBM
// date: 2016/02/01
// base class for walker

#ifndef WALKERBASE_H
#define WALKERBASE_H

#include "Vec3.hpp"
#include "ParameterReader.h"
#include "Walker.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
class WalkerBase : public Walker
{
public:
  // member functions
  void setProperties(ParameterReader& pr, string cloudID);
  void write(string filename);

  // constructor
  WalkerBase(double xi, double yi, double zi) : 
    r_(1.0), wallHit_(0), lastHitAge_(0.0) {
    Vec3<double> p{xi, yi, zi};
    //cout << "[Walker(" << count_ << ")] is initialized at " << position_ << endl;
    position(p);
    age(0.0);
    trace(0.0);
    ID("WalkerBase");
    positionn_ = p;
    count_++;
  }
  WalkerBase(Vec3<double> v) : WalkerBase(v.X(), v.Y(), v.Z()) { }
  WalkerBase() : WalkerBase(0.0, 0.0, 0.0) { }
  virtual ~WalkerBase() { }

  // inline member functions
  //Vec3<double> position0() { return position0_; }
  //void position0(Vec3<double> v) { position0_ = v; }
  inline double r() { return r_; }
  inline void r(double r) { r_=r; }
  inline double volume() { return volume_; }
  inline void volume(double v) { volume_ = v; }

  inline size_t wallHit() { return wallHit_; }
  inline void wallHit(size_t h) { wallHit_ = h; }
  inline void addWallHit(size_t i) { wallHit_ += i; }

  inline size_t substrateHit() { return 0; }
  inline void addSubstrateHit(size_t hit) { return; }
  inline double duration() { return 0; }
  inline void duration(double d) { return; }
  inline void subDuration(double dt) { return; }
  inline double lastHitAge() { return lastHitAge_; }
  inline void lastHitAge(double a) { lastHitAge_ = a; }
  inline Vec3<double> lastHitPosition() { return lastHitPosition_; }
  inline void lastHitPosition(Vec3<double> p) { lastHitPosition_ = p; }

  inline double mass() { return 0.0; }

protected:
  double r_;
  size_t wallHit_;
  double volume_;
  double lastHitAge_;

  Vec3<double> lastHitPosition_;
  // reserved for MSD calculation
  //Vec3<double> position0_;

private:

};
/////////////////////////////////////////////////////////////////////////////
#endif

// vim: foldmethod=syntax:foldlevel=1
