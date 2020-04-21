// CellGeo.h
// class for cell geometry
//
// author: sungcheolkim @ IBM
// date: 20160618 version: 1.0.0
// date: 20160630 version: 1.1.0 update: 4 types of active site distribution

#ifndef CELLSURFACES_H
#define CELLSURFACES_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_const_num.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "ParameterReader.h"
#include "Surfaces.hpp"
#include "Vec3.hpp"

using namespace std;

class SurfacesCell : public Surfaces {
public:
  // member functions
  bool isInside(float x, float y, float z);
  inline bool isInside(Vec3<double> v) { return isInside(v.X(), v.Y(), v.Z()); }
  Vec3<double> calRandomPosition(gsl_rng* rs, SurfaceTypeClass sc);
  Vec3<double> calRandomPosition(gsl_rng* rs);

  bool isInsideVol(float x, float y, float z, float l);
  inline bool isInsideVol(float x, float y, float z) { return isInsideVol(x, y, z, radius_); }
  inline bool isInsideSur(float x, float y, float z) {
    return ((isInsideVol(x,y,z,radius_)) and !isInsideVol(x,y,z,radius_*(1.0-ringDepth_)));
  };
  bool isInsideDisk(float x, float y, float z);
  bool isInsideRing(float x, float y, float z);

  Vec3<double> calVolPosition(gsl_rng* rs);
  Vec3<double> calSurPosition(gsl_rng* rs);
  Vec3<double> calDiskPosition(gsl_rng* rs);
  Vec3<double> calRingPosition(gsl_rng* rs);

  Vec3<double> calNormal(Vec3<double> p);
  double calSurfaceDistance(Vec3<double> p);

  // constructor
  SurfacesCell(ParameterReader& pr, string cID): length_(0), radius_(0), bandPosition_(0), bandWidth_(0), ringDepth_(0) {
    cloudID(cID);
    surfaceID("Bacteria Surfaces");
    cout << blu << "[" << surfaceID() << "(" << cloudID() << ")] is initialized." << def << endl;

    length_ = pr.doubleRead(cID+" Cell Length", "6");
    radius_ = pr.doubleRead(cID+" Cell Radius", "1");
    debug(pr.boolRead(cID+" Debug", "False"));

    pradius(pr.doubleRead(cloudID_+" Particle Radius", "1")/1000.0);

    volume_ = 4.0/3.0*M_PI*radius_*radius_*radius_ + radius_*radius_*M_PI*length_;
    cout << "... cal Total Volume: " << gre << volume_ << def << " [um3]" << endl;

    // get band information for ring, disk, surface cases
    surfaceType(pr.stringRead(cID+" Surface Type", "Cell"));
    if (pr.checkName(cloudID_+" Band Position"))
      bandPosition_ = pr.doubleRead(cloudID_+" Band Position", "0.0");
    else bandPosition_ = 0.0;
    if (pr.checkName(cloudID_+" Band Width"))
      bandWidth_ = pr.doubleRead(cloudID_+" Band Width", "0.2");
    else bandWidth_ = 0.2;
    if (pr.checkName(cloudID_+" Ring Depth")) {
      ringDepth_ = pr.doubleRead(cloudID_+" Ring Depth", "0.1");
    } else ringDepth_ = 0.1;
    if (pr.checkName(cloudID_+" Ring Number")) {
      ringNumber_ = pr.intRead(cloudID_+" Ring Number", "1");
    } else ringNumber_ = 1;

    // ring case
    if (surfaceType().find("ring") != string::npos) {
      stype(SurfaceTypeClass::ring);
      typeVolume(M_PI*radius_*radius_*(2.0-ringDepth_)*ringDepth_*bandWidth_*length_);
      surfaceArea(4.0*M_PI*radius_*radius_*(2.0*ringDepth_-ringDepth_*ringDepth_)+2.0*M_PI*radius_*(2.0-ringDepth_)*length_*bandWidth_);
    // disk case
    } else if (surfaceType().find("disk") != string::npos) {
      stype(SurfaceTypeClass::disk);
      typeVolume(M_PI*radius_*radius_*bandWidth_*length_);
      surfaceArea(2.0*M_PI*radius_*radius_+2.0*M_PI*radius_*length_*bandWidth_);
    // surface case
    } else if (surfaceType().find("sur") != string::npos) {
      stype(SurfaceTypeClass::surface);
      typeVolume(4.0/3.0*M_PI*radius_*radius_*radius_*ringDepth_*(3.0-3.0*ringDepth_+ringDepth_*ringDepth_) + radius_*radius_*M_PI*length_*(2.0-ringDepth_)*ringDepth_);
      surfaceArea(4.0*M_PI*radius_*radius_+2.0*M_PI*radius_*length_+
                  4.0*M_PI*radius_*radius_*(1.0-ringDepth_)*(1.0-ringDepth_)+
                  2.0*M_PI*radius_*(1.0-ringDepth_)*length_);
    // volume case
    } else {
      stype(SurfaceTypeClass::volume);
      typeVolume(4.0/3.0*M_PI*radius_*radius_*radius_ + radius_*radius_*M_PI*length_);
      surfaceArea(4.0*M_PI*radius_*radius_+2.0*M_PI*radius_*length_);
    }
    cout << "... cal Cloud Volume: " << gre << typeVolume() << def << " [um3]" << endl;
    cout << "... cal Cloud Surface Area: " << gre << surfaceArea() << def << " [um2]" << endl;
  }
  virtual ~SurfacesCell() {};

  // inline functions
  inline float length() { return length_; }
  inline float radius() { return radius_; }
  inline float bandPosition() { return bandPosition_; }
  inline float bandWidth() { return bandWidth_; }
  inline float ringDepth() { return ringDepth_; }
  inline size_t ringNumber() { return ringNumber_; }
  inline Vec3<double> maxDimension() {
    return Vec3<double>{length_/2.0+radius_, radius_, radius_}; }
  inline Vec3<double> minDimension() {
    return Vec3<double>{-length_/2.0-radius_, -radius_, -radius_}; }

private:
  float length_;
  float radius_;

  float bandPosition_;
  float bandWidth_;
  float ringDepth_;
  size_t ringNumber_;

};

bool SurfacesCell::isInsideVol(float x, float y, float z, float radius) {
    // container dependent function
    // for cell, 8um length, 2um wide

    Vec3<double> leftc(-length_/2.0, 0.0, 0.0);
    Vec3<double> rightc(length_/2.0, 0.0, 0.0);
    Vec3<double> p(x, y, z);

    if(x < -length_/2.0)
        if((p-leftc).mag() <= radius-pr_)
            return true;

    if(x > length_/2.0)
        if((p-rightc).mag() <= radius-pr_)
            return true;

    if(x >= -length_/2.0 && x <= length_/2.0)
        if(sqrt(y*y + z*z) <= radius-pr_)
            return true;

    return false;
}

bool SurfacesCell::isInsideDisk(float x, float y, float z) {
  if ((x>length_/2.0-bandPosition_*length_-bandWidth_*length_) and (x<length_/2.0-bandPosition_*length_))
    if (sqrt(y*y + z*z) <= radius_-pr_)
      return true;

  return false;
}

bool SurfacesCell::isInsideRing(float x, float y, float z) {
  // check particle center is inside a ring domain
  bool flag = false;
  float x0; 
  for (size_t i=0; i < ringNumber_; ++i) {
    if (ringNumber_ > 1)
      x0 = length_*0.5 + (float)(i) * length_*(1.0 - bandWidth_)/((float)(ringNumber_)- 1.0) - length_*bandWidth_*0.5;
    else
      x0 = length_*0.5 - length_*bandWidth_*0.5;

    //cout << "... x0: " << x0 << endl;
    if (abs(x - x0) < length_*bandWidth_*0.5) flag = true;
  }

  if (flag) {
      double hr = sqrt(y*y + z*z);
      if ((hr>=radius_*(1.0-ringDepth_)-pr_) and (hr<=radius_-pr_))
        return true;
  }
  return false;
}

bool SurfacesCell::isInside(float x, float y, float z) {
  switch(stype()) {
    case SurfaceTypeClass::volume: return isInsideVol(x, y, z);
    case SurfaceTypeClass::surface: return isInsideSur(x, y, z);
    case SurfaceTypeClass::disk: return isInsideDisk(x, y, z);
    case SurfaceTypeClass::ring: return isInsideRing(x, y, z);
  }
}

Vec3<double> SurfacesCell::calVolPosition(gsl_rng* rs) {
    float x,y,z;
    do {
        x = gsl_rng_uniform(rs)*(length_+2.0*radius_-2.0*pr_)-(length_/2.0+radius_+pr_);
        y = (gsl_rng_uniform(rs)*2.0 - 1.0)*(radius_ - pr_);
        z = (gsl_rng_uniform(rs)*2.0 - 1.0)*(radius_ - pr_);
    } while(!isInsideVol(x,y,z));

    return Vec3<double>{x,y,z};
}

Vec3<double> SurfacesCell::calSurPosition(gsl_rng* rs) {
    float x, y, z;
    do {
      x = gsl_rng_uniform(rs)*(length_+2.0*radius_)-(length_/2.0+radius_);
      y = (gsl_rng_uniform(rs)*2.0 - 1.0)*(radius_ - pr_);
      z = (gsl_rng_uniform(rs)*2.0 - 1.0)*(radius_ - pr_);
    } while(!isInsideSur(x,y,z));

    return Vec3<double>{x,y,z};
}

Vec3<double> SurfacesCell::calDiskPosition(gsl_rng* rs) {
    double x = length_/2.0 - bandPosition_*length_ - bandWidth_*length_*gsl_rng_uniform(rs);
    double y, z;

    do {
      y = (gsl_rng_uniform(rs)*2.0 - 1.0)*(radius_ - pr_);
      z = (gsl_rng_uniform(rs)*2.0 - 1.0)*(radius_ - pr_);
    } while(!isInsideDisk(x,y,z));

    return Vec3<double>{x, y, z};
}

Vec3<double> SurfacesCell::calRingPosition(gsl_rng* rs) {
    double x, y, z;

    do {
      x = length_/2.0 - length_*gsl_rng_uniform(rs);
      y = (gsl_rng_uniform(rs)*2.0 - 1.0)*(radius_ - pr_);
      z = (gsl_rng_uniform(rs)*2.0 - 1.0)*(radius_ - pr_);
      //cout << "... not inside " << x << ", " << y << ", " << z << endl;
    } while(!isInsideRing(x,y,z));

    return Vec3<double>{x, y, z};
}

Vec3<double> SurfacesCell::calRandomPosition(gsl_rng* rs, SurfaceTypeClass sc) {
  switch(sc) {
    case SurfaceTypeClass::volume: return calVolPosition(rs);
    case SurfaceTypeClass::surface: return calSurPosition(rs);
    case SurfaceTypeClass::disk: return calDiskPosition(rs);
    case SurfaceTypeClass::ring: return calRingPosition(rs);
  }
}

Vec3<double> SurfacesCell::calRandomPosition(gsl_rng* rs) {
  switch(stype()) {
    case SurfaceTypeClass::volume: return calVolPosition(rs);
    case SurfaceTypeClass::surface: return calSurPosition(rs);
    case SurfaceTypeClass::disk: return calDiskPosition(rs);
    case SurfaceTypeClass::ring: return calRingPosition(rs);
  }
}

Vec3<double> SurfacesCell::calNormal(Vec3<double> p) {
  // computer normal vector

  Vec3<double> n, v;
  // inside cylinder
  if ((p.X()>-length_/2.0) and p.X()<length_/2.0) {
    n.setXYZ(0, -p.Y(), -p.Z());
    n.normalise();
    return n;
  }
  // inside hemisphere
  if (p.X() > 0)
    v.set(length_/2.0, 0, 0);
  else
    v.set(-length_/2.0, 0, 0);
  n = (v - p);
  n.normalise();
  return n;
}

double SurfacesCell::calSurfaceDistance(Vec3<double> p) {
  // compute distance between position and wall
  
  Vec3<double> n, v;

  // inside cylinder
  if ((p.X()>-length_/2.0) and p.X()<length_/2.0) {
    n.set(0, p.Y(), p.Z());
    return radius_ - n.mag();
  }
  // inside hemisphere
  if (p.X() > 0)
    v.set(length_/2.0, 0, 0);
  else
    v.set(-length_/2.0, 0, 0);
  n = p - v;
  return radius_ - n.mag();
}
#endif

// vim:foldmethod=syntax:foldlevel=0
