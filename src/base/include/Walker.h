// Walker.h - abstract class
//
// author: sungcheolkim @ IBM
// date: 2016/02/01
// date: 2017/09/12 - reconstruct class structure

#ifndef WALKER_H
#define WALKER_H

#include "Vec3.hpp"
#include "ParameterReader.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
class Walker
{
public:
  // virtual functions
  virtual void setProperties(ParameterReader& pr, string cloudID) = 0;
  virtual void write(string filename) = 0;

  virtual void addWallHit(size_t i) = 0;
  virtual size_t wallHit() = 0;
  virtual size_t substrateHit() = 0;
  virtual void addSubstrateHit(size_t hit) = 0;
  virtual double duration() = 0;
  virtual void duration(double d) = 0;
  virtual void subDuration(double dt) = 0;
  virtual double r() = 0;
  virtual double volume() = 0;
  virtual double mass() = 0;
  virtual double lastHitAge() = 0;
  virtual void lastHitAge(double a) = 0;
  virtual Vec3<double> lastHitPosition() = 0;
  virtual void lastHitPosition(Vec3<double> p) = 0;

  // member functions
  void step(Vec3<double> dr);
  size_t calPID(Vec3<double> p);
  inline size_t calPID() { return calPID(position_); }
  void setPIDRange(double x1, double x2, double x3, double y1, double z1);

  // inline functions
  inline Vec3<double> position() { return position_; }
  inline void position(Vec3<double> p) { positionn_ = position_; position_ = p; }
  inline Vec3<double> positionn() { return positionn_; }
  inline double age() { return age_; }
  inline void age(double a) { age_ = a; }
  inline void addAge(double dt) { age_ += dt; }
  inline double trace() { return trace_; }
  inline void trace(double l) { trace_ = l; }
  inline void addTrace(double l) { trace_ += l; }
  inline size_t tid() { return tid_; }
  inline void tid(size_t t) { tid_ = t; }
  inline size_t pid() { return pid_; }
  inline void pid(size_t t) { pid_ = t; }
  inline string ID() { return walkerID_; }
  inline void ID(string s) { walkerID_ = s; }
  inline size_t walkerSize() { return count_; }
  inline string cloudID() { return cloudID_; }
  inline void cloudID(string s) { cloudID_ = s; }

protected:
  string walkerID_;
  string cloudID_;
  Vec3<double> position_;
  Vec3<double> positionn_;
  double age_;
  double trace_;
  size_t tid_;

  size_t pid_;    // process id for virtual space
  double px1_, px2_, px3_;
  double py1_;
  double pz1_;

  static unsigned int count_;

};
/////////////////////////////////////////////////////////////////////////////
#endif

// vim: foldmethod=syntax:foldlevel=1
