// SurfacesBox.hpp - Surface class for Box
//
// author: Sung-Cheol Kim @ IBM
// date: 2017/09/09 - derived from Surfaces.hpp
// date: 20170912 - clean up using abstract class

#ifndef SURFACES_BOX_H
#define SURFACES_BOX_H

#include "Vec3.hpp"
#include "ParameterReader.h"
#include "Surfaces.hpp"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;

class SurfacesBox: public Surfaces {

public:
  // member functions
  bool isInside(float x, float y, float z);
  inline bool isInside(Vec3<double> v) { return isInside(v.X(), v.Y(), v.Z()); }
  Vec3<double> calRandomPosition(gsl_rng* rs, SurfaceTypeClass sc=SurfaceTypeClass::volume);
  inline Vec3<double> calRandomPosition(gsl_rng* rs) { return calRandomPosition(rs, SurfaceTypeClass::volume); }
  Vec3<double> calNormal(Vec3<double> position);
  double calSurfaceDistance(Vec3<double> position);

  // constructor
  SurfacesBox(ParameterReader& pr, string cID): width_(0), length_(0), depth_(0) {
    cloudID(cID);
    surfaceID("Box Surfaces");
    surfaceType("vol");
    cout << blu << "[" << surfaceID() << "(" << cloudID() << ")] is initialized." << def << endl;

    Vec3<double> v = pr.vec3Read(cloudID_+" Box Dimensions", "(1, 1, 1)");
    width_ = v.X();
    length_ = v.Y();
    depth_ = v.Z();
    volume(width_*length_*depth_);
    typeVolume(width_*length_*depth_);
    surfaceArea(2.0*(width_*length_+length_*depth_+depth_*width_));
    cout << "... cal Total Volume: " << gre << volume() << def << " [um3]" << endl;
    cout << "... cal Surface Area: " << gre << surfaceArea() << def << " [um2]" << endl;

    pr_ = pr.doubleRead(cloudID_+" Particle Radius", "1")/1000.0;
  };
  virtual ~SurfacesBox() {};

  // inline functions
  inline double width() { return width_; }
  inline double length() { return length_; }
  inline double depth() { return depth_; }
  inline double pr() { return pr_; }
  inline Vec3<double> maxDimension() { return Vec3<double>{width_/2.0, length_/2.0, depth_/2.0}; }
  inline Vec3<double> minDimension() { return Vec3<double>{-width_/2.0, -length_/2.0, -depth_/2.0}; }

private:
  double width_;
  double length_;
  double depth_;

  double pr_;       // particle radius
};

bool SurfacesBox::isInside(float x, float y, float z) {
  Vec3<double> p(x, y, z);
  if ( x < -width_/2.0+pr_ or x > width_/2.0-pr_)
    return false;
  if ( y < -length_/2.0+pr_ or y > length_/2.0-pr_)
    return false;
  if ( z < -depth_/2.0+pr_ or z > depth_/2.0-pr_)
    return false;

  return true;
}

Vec3<double> SurfacesBox::calRandomPosition(gsl_rng* rs, SurfaceTypeClass sc) {
  double x, y, z;

  do {
    x = gsl_rng_uniform(rs)*width_-width_/2.0;
    y = gsl_rng_uniform(rs)*length_-length_/2.0;
    z = gsl_rng_uniform(rs)*depth_-depth_/2.0;
  } while(!isInside(x, y, z));

  Vec3<double> p(x,y,z);
  return p;
}

Vec3<double> SurfacesBox::calNormal(Vec3<double> p) {
  // compute normal vector
  Vec3<double> n{0, 0, 0};

  double resx, resy, resz;
  resx = fabs(fabs(p.X()) - width_/2.0);
  resy = fabs(fabs(p.Y()) - length_/2.0);
  resz = fabs(fabs(p.Z()) - depth_/2.0);

  //cerr << std::setprecision(6) << "... res x,y,z " << resx << " " << resy << " " << resz << endl;
  //cerr << "... px: " << p.X() << " py: " << p.Y() << " pz: " << p.Z() << endl;
  //cerr << "... pr: " << pr_ << endl;

  // check position - %1 allowance of pr (1.01)
  if (resx <= pr_*1.01) 
    n.X((p.X() > 0) ? -1.0 : 1.0);
  if (resy <= pr_*1.01)
    n.Y((p.Y() > 0) ? -1.0 : 1.0);
  if (resz <= pr_*1.01)
    n.Z((p.Z() > 0) ? -1.0 : 1.0);

  n.normalise();

  return n;
}

double SurfacesBox::calSurfaceDistance(Vec3<double> p) {
  double resx, resy, resz, res;

  resx = width_/2.0 - fabs(p.X());
  resy = length_/2.0 - fabs(p.Y());
  resz = depth_/2.0 - fabs(p.Z());
  res = resx;

  if (res > resy) res = resy;
  if (res > resz) res = resz;

  return res;
}
#endif

// vim:foldmethod=syntax:foldlevel=0
