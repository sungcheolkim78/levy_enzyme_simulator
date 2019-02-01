// Walker.cpp
// base class for walker
//
// author: sungcheolkim @ IBM
// date: 2016/02/01
// date: 20160619 version: 1.1.0 update: reconcept class
// date: 20170929 - add pid check
// date: 20171005 - modify pid check routine

#include "../include/Walker.h"

void Walker::step(Vec3<double> dr) {
  // record previous position
  positionn_ = position_;
  // update pid
  pid(calPID(position_+dr));
  // move to next position
  position_ += dr;

  //trace_ += dr.mag();
}

size_t Walker::calPID(Vec3<double> p) {
  size_t xind = 0;
  size_t yind = 0;
  size_t zind = 0;

  // pid range = (0, 7)
  if (p.X() >= px3_) { xind = 0; }
  else if (p.X() >= px2_) { xind = 1; }
  else if (p.X() >= px1_) { xind = 2; }
  else { xind = 3; }

  if (p.Y() < py1_) yind = 1;
  if (p.Z() < pz1_) zind = 1;

  return xind + 4*yind + 8*zind;
}

void Walker::setPIDRange(double x1, double x2, double x3, double y1, double z1) {
  px1_ = x1; px2_ = x2; px3_ = x3;
  py1_ = y1;
  pz1_ = z1;
}

unsigned int Walker::count_ = 0;

// vim:foldmethod=syntax:foldlevel=0
