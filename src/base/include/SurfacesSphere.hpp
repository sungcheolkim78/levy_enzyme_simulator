// SurfacesSphere.hpp
// spherical surfaces and their interactions
//
// author: Sung-Cheol Kim @ IBM
// date: 2017/09/09 - derived from Surfaces.hpp
//

#ifndef SURFACES_SPHERE_H
#define SURFACES_SPHERE_H

#include "Vec3.hpp"
#include "ParameterReader.h"
#include "Surfaces.hpp"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>

using namespace std;

class SurfacesSphere: public Surfaces {

public:
  // member functions
  bool isInside(float x, float y, float z);
  inline bool isInside(Vec3<double> v) { return isInside(v.X(), v.Y(), v.Z()); }
  Vec3<double> calRandomPosition(gsl_rng* rs, SurfaceTypeClass sc=SurfaceTypeClass::volume);
  inline Vec3<double> calRandomPosition(gsl_rng* rs) { return calRandomPosition(rs, SurfaceTypeClass::volume); }
  Vec3<double> calNormal(Vec3<double> p);
  double calSurfaceDistance(Vec3<double> p);

  // constructor
  SurfacesSphere(ParameterReader& pr, string cID): radius_(0) {
    cloudID(cID);
    surfaceID("Sphere Surfaces");
    surfaceType("vol");
    cout << "[" << surfaceID() << "(" << cloudID() << ")] is initialized." << endl;

    radius_ = pr.doubleRead(cID+" Sphere Radius", "1");
    volume(4.0/3.0*M_PI*radius_*radius_*radius_);
    typeVolume(4.0/3.0*M_PI*radius_*radius_*radius_);
    surfaceArea(4.0*M_PI*radius_*radius_);
    cout << "... total volume: " << volume() << " [um3]" << endl;

    pr_ = pr.doubleRead(cloudID_+" Particle Radius", "1")/1000.0;
  };
  virtual ~SurfacesSphere() {};

  // inline functions
  inline double radius() { return radius_; }
  inline double pr() { return pr_; }
  inline Vec3<double> maxDimension() { return Vec3<double>{radius_, radius_, radius_}; }
  inline Vec3<double> minDimension() { return Vec3<double>{-radius_, -radius_, -radius_}; }

private:
  double radius_;
  double pr_;       // particle radius
};

bool SurfacesSphere::isInside(float x, float y, float z) {
  Vec3<double> p(x, y, z);
  if (p.mag() < radius_-pr_)
    return true;
  else
    return false;
}

Vec3<double> SurfacesSphere::calRandomPosition(gsl_rng* rs, SurfaceTypeClass sc) {
  double x, y, z;

  do {
    x = (gsl_rng_uniform(rs)*2.0-1.0)*radius_;
    y = (gsl_rng_uniform(rs)*2.0-1.0)*radius_;
    z = (gsl_rng_uniform(rs)*2.0-1.0)*radius_;
  } while(!isInside(x, y, z));

  Vec3<double> p(x,y,z);
  return p;
}

Vec3<double> SurfacesSphere::calNormal(Vec3<double> p) {
  // compute normal vector
  Vec3<double> n{0.0, 0.0, 0.0};
  
  // check p is on surface with 1% allowance 
  if (p.mag() > (radius_-pr_)*1.01) {
    cerr << std::setprecision(6) << "... p: " << p << " pmag: " << p.mag() << " r-pr: " << radius_-pr_*1.01 << endl;
    return n;
  }

  n = p * (-1.0);
  n.normalise();
  return n;
}

double SurfacesSphere::calSurfaceDistance(Vec3<double> p) {
  return radius_ - p.mag();
}
#endif

// vim:foldmethod=syntax:foldlevel=0
