// Surfaces.h - abstract class for surfaces
//
// author: Sung-Cheol Kim @ IBM
// date: 2017/09/07 - derived from cellgeo.h
//

#ifndef SURFACES_H
#define SURFACES_H

#include "Vec3.hpp"
#include "ParameterReader.h"
#include "colormod.h"
#include <gsl/gsl_rng.h>
#include <iomanip>

using namespace std;

Color::Modifier red(Color::FG_RED);
Color::Modifier gre(Color::FG_GREEN);
Color::Modifier blu(Color::FG_BLUE);
Color::Modifier def(Color::FG_DEFAULT);

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
  Vec3<double> calNewStep(Vec3<double> position, Vec3<double> dr, double tt, int count);

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
  inline bool debug() { return debug_; }
  inline void debug(bool t) { debug_ = t; }

protected:
  string cloudID_;
  string surfaceID_;
  string surfaceType_;
  SurfaceTypeClass stype_;
  double volume_;
  double typeVolume_;
  double surfaceArea_;
  double pr_;
  bool debug_;

private:

};

double Surfaces::getTimeForSurface(Vec3<double> position, Vec3<double> dr) {
  // particle position: p
  // movement vector: dr
  // output: number tt - p+tt*dr always on the wall
  // condition: p should be inside, p+dr should be outside

  if (!isInside(position)) {
    if (debug_)
      cerr << red << "... calculate surface distance from outside p=" << position << def << endl;
    exit(1);
  }
  if (isInside(position+dr)) {
    if (debug_)
      cerr << red << "... calculate surface distance from too far p=" << position << " dr=" << dr << def << endl;
    return 2.0;
  }

  //if (debug())
  //  cerr << gre << std::setprecision(3) << "... [Wall] p=" << position << " dr= " << dr << def << endl;

  // binary search for surface distance on dr line
  const size_t order{10};     // precision 1/2^10 = 1/1024 ~ 0.001
  size_t count_p{0}, count_n{0};
  double dp{0.0}, dp_n{0.5}, dp_o{0.0};
  for (size_t i=0; i < order; ++i) {
    
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
    if (debug())
      cerr << gre << std::setprecision(3) << "... [getTimToSurface] p=" << position << " dr=" << dr << " dp=" << dp << def << endl;
  } else {
    if (debug()) {
      cerr << gre << "... [getTimeToSurface][" << count_p + count_n << "] dp=" << dp
            << std::setprecision(3) << std::scientific << "(" << dp_n << std::defaultfloat << ") c+="
            << count_p << " c-=" << count_n << def << endl;
    }
  }

  // find hitting point and stop there
  // tt = 0 - hit surface already
  // tt < 1.0 - hit surface before dr
  // tt = 2.0 - hit surface after dr

  return dp;
}

Vec3<double> Surfaces::calNewStep(Vec3<double> position, Vec3<double> dr, double tt, int count=0) {
  // check it is still inside
  if (isInside(position + dr)) {
    count = 0;
    return dr;
  }

  // find wall hit position
  Vec3<double> n = calNormal(position + dr*tt);
  if (n.mag() == 0) return dr*tt;

  // calculate new step vector
  Vec3<double> vn = dr - n*(2.0*(1.0-tt)*(dr.dotProduct(n)));
  //if (debug())
  //  cerr << red << "... [calNewStep] position: " << position << " normal: " << n << " step: " << vn << def << endl;

  // almost hit wall
  if (vn.mag() == 0) {
    if (debug_)
      cerr << red << "... [calNewStep] no new step - position: " << position << def << endl;
    return dr*tt;
  }
  // hit wall from other wall position
  if (dr.dotProduct(n) < 0.0000001) {
    vn = dr*tt + n*((1.0-tt)*dr.mag());
    if (debug_)
      cerr << red << "... [calNewStep] dr perpendicular to n - dr: " << dr << " n: " << n << def << endl;
  }

  // check inside
  if (isInside(position + vn)) {
    count = 0;
    return vn;
  } else {
    // if new step is outside, recurive call 
    // 20200422 - this recursive algorithm does not match with MM rate 
    tt = getTimeForSurface(position, vn);
    //if (debug()) {
    //  cerr << red << std::setprecision(3);
    //  cerr << "... [calNewStep] outside - recursive call - collision angle :" << dr.dotProduct(n)/dr.mag() << endl;
    //  cerr << "... [calNewStep] new step: " << vn << "  tt: " << tt << def << endl;
    // }
    // limit recursive call upto 3 times
    count++;
    if (count > 2) return vn*tt;
    else return calNewStep(position, vn, tt, count);
    // or hit wall and stay until new direction is inside cell
    //count = 0;
    //return dr*tt;
  }
}

#endif

// vim:foldmethod=syntax:foldlevel=0
