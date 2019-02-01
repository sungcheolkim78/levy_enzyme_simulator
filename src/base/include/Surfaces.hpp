// Surfaces.h - abstract class for surfaces
//
// author: Sung-Cheol Kim @ IBM
// date: 2017/09/07 - derived from cellgeo.h
//

#ifndef SURFACES_H
#define SURFACES_H

#include "Vec3.hpp"
#include "ParameterReader.h"
#include <gsl/gsl_rng.h>
#include <iomanip>

using namespace std;

enum class SurfaceTypeClass { volume, surface, disk, ring };

class Surfaces {

public:
  // virtual functions
  virtual bool isInside(float x, float y, float z) = 0;
  virtual Vec3<double> calRandomPosition(gsl_rng* rs) = 0;
  virtual Vec3<double> calRandomPosition(gsl_rng* rs, SurfaceTypeClass sc) = 0;
  virtual Vec3<double> calNormal(Vec3<double> position) = 0;
  virtual Vec3<double> maxDimension() = 0;
  virtual Vec3<double> minDimension() = 0;
  virtual double calSurfaceDistance(Vec3<double> position) = 0;

  // member functions
  double getTimeForSurface(Vec3<double> position, Vec3<double> dr);
  inline bool isInside(Vec3<double> p) { return isInside(p.X(), p.Y(), p.Z()); }
  Vec3<double> calNewStep(Vec3<double> position, Vec3<double> dr, double tt);

  // inline functions
  string cloudID() { return cloudID_; }
  void cloudID(string s) { cloudID_ = s; }
  string surfaceID() { return surfaceID_; }
  void surfaceID(string s) { surfaceID_ = s; }
  string surfaceType() { return surfaceType_; }
  void surfaceType(string s) { surfaceType_ = s; }
  double volume() { return volume_; }
  void volume(double v) { volume_ = v; }
  double typeVolume() { return typeVolume_; }
  void typeVolume(double v) { typeVolume_ = v; }
  SurfaceTypeClass stype() { return stype_; }
  void stype(SurfaceTypeClass s) { stype_ = s; }
  double pradius() { return pr_; }
  void pradius(double r) { pr_ = r; }
  double surfaceArea() { return surfaceArea_; }
  void surfaceArea(double a) { surfaceArea_ = a; }

protected:
  string cloudID_;
  string surfaceID_;
  string surfaceType_;
  SurfaceTypeClass stype_;
  double volume_;
  double typeVolume_;
  double surfaceArea_;
  double pr_;

private:

};

double Surfaces::getTimeForSurface(Vec3<double> position, Vec3<double> dr) {
  // particle position: p
  // movement vector: dr
  // output: number tt - p+tt*dr always on the wall
  // condition: p should be inside, p+dr should be outside

  if (!isInside(position)) {
    cerr << "... calculate surface distance from outside p=" << position << endl;
    exit(1);
  }
  if (isInside(position+dr)) {
    //cerr << "... calculate surface distance from too far p=" << position << " dr=" << dr << endl;
    return 2.0;
  }

  //cerr << std::setprecision(5) << "... [Wall] p=" << position << " dr= " << dr << endl;
  // binary search for surface distance on dr line
  const size_t order{10};     // precision 1/2^10 = 1/1024 ~ 0.001
  size_t count_p{0}, count_n{0};
  double dp{0.0}, dp_n{0.5}, dp_o{0.0};
  for (size_t i=0; i < order; ++i) {
    /*
    cerr << "... [Surface][" << count_p + count_n << "] dp=" << dp
         << std::setprecision(5) << std::scientific << "(" << dp_n << std::defaultfloat << ") c+="
         << count_p << " c-=" << count_n << endl;
    */

    dp += dp_n;

    dp_n *= 0.5;
    if (isInside(position+dr*dp)) {
      dp_n = fabs(dp_n);
      count_p++;
      dp_o = dp;
    } else {
      dp_n = -fabs(dp_n);
      count_n++;
    }
  }

  if (dp_n < 0) {
    dp = dp_o;
    //cerr << std::setprecision(5) << "... [gTFS] p=" << position << " dr=" << dr << " dp=" << dp << endl;
  }

  // find hitting point and stop there
  // tt = 0 - hit surface already
  // tt < 1.0 - hit surface before dr
  // tt = 2.0 - hit surface after dr

  return dp;
}

Vec3<double> Surfaces::calNewStep(Vec3<double> position, Vec3<double> dr, double tt) {
  // check it is still inside
  if (isInside(position + dr)) return dr;

  // find wall hit position
  Vec3<double> n = calNormal(position + dr*tt);
  //cerr << "... new step, position: " << position << " normal: " << n << endl;
  if (n.mag() == 0) return dr*tt;

  Vec3<double> vn = (dr - n*(2.0*(1.0-tt)*(dr.dotProduct(n))));
  if (vn.mag() == 0) return dr*tt;

  // check inside
  if (isInside(position + vn)) {
    return vn;
  } else {
    // if new step is outside, recurive call 
    tt = getTimeForSurface(position, vn);
    //cerr << std::setprecision(6);
    //cerr << "... recursive call: " << position << " old step: " << dr << " normal: " << n << endl;
    //cerr << " new step: " << vn << "  tt: " << tt << endl;
    return calNewStep(position, vn, tt);
  }
}

#endif

// vim:foldmethod=syntax:foldlevel=0
