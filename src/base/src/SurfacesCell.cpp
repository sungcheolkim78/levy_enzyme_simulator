////////////////////////////////////////////////////////////////////////////
// CellGeo.cpp
// implementation of cellgeo class - related to geometry, module
//
// author: sungcheolkim @ IBM
// date: 20160619 version: 1.0.0

#include "../include/SurfacesCell.h"

using namespace std;

bool SurfacesCell::isInside(float x, float y, float z, float length) {
    // container dependent function
    // for cell, 8um length, 2um wide

    Vec3<double> leftc(-length/2.0, 0.0, 0.0);
    Vec3<double> rightc(length/2.0, 0.0, 0.0);
    Vec3<double> p(x, y, z);

    if(x < -length/2.0)
        if((p-leftc).mag() <= radius_)
            return true;

    if(x > length/2.0)
        if((p-rightc).mag() <= radius_)
            return true;

    if(x >= -length/2.0 && x <= length/2.0)
        if(gsl_hypot(x, z) <= radius_)
            return true;

    return false;
}

Vec3<double> SurfacesCell::calVolPosition(gsl_rng* rs) {
    float x,y,z;
    do {
        x = gsl_rng_uniform(rs)*(length_+2.0*radius_)-(length_/2.0+radius_);
        y = gsl_rng_uniform(rs)*2.0*radius_ - radius_;
        z = gsl_rng_uniform(rs)*2.0*radius_ - radius_;
    } while(!isInside(x,y,z));

    Vec3<double> position(x,y,z);
    return position;
}

Vec3<double> SurfacesCell::calSurPosition(gsl_rng* rs) {
    float x, y, z;
    double nradius = radius_ * (1.0 - ringDepth_);

    do {
      do
      {
          x = gsl_rng_uniform(rs)*(length_+2.0*radius_)-(length_/2.0+radius_);
          y = gsl_rng_uniform(rs)*2.0*radius_ - radius_;
          z = gsl_rng_uniform(rs)*2.0*radius_ - radius_;
      } while(isInside(x, y, z, nradius));
    } while(!isInside(x,y,z));

    Vec3<double> position(x, y, z);
    return position;
}

Vec3<double> SurfacesCell::calDiskPosition(gsl_rng* rs) {
    double x = length_/2.0 - bandPosition_*length_ - bandWidth_*length_*gsl_rng_uniform(rs);

    double y, z;
    do
    {
        y = gsl_rng_uniform(rs)*2.0*radius_ - radius_;
        z = gsl_rng_uniform(rs)*2.0*radius_ - radius_;
    } while(!isInside(x,y,z));

    Vec3<double> position(x, y, z);
    return position;
}

Vec3<double> SurfacesCell::calRingPosition(gsl_rng* rs) {
    double x = length_/2.0 - bandPosition_*length_ - bandWidth_*length_*gsl_rng_uniform(rs);
    double theta = 2.0*M_PI*gsl_rng_uniform(rs);

    double l = (ringDepth_*gsl_rng_uniform(rs) + 1.0 - ringDepth_) * radius_;
    double y = l * gsl_sf_sin(theta);
    double z = l * gsl_sf_cos(theta);

    Vec3<double> position(x, y, z);
    return position;
}

Vec3<double> SurfacesCell::calRandomPosition(gsl_rng* rs) {
    if (surfaceType_.find("ring") != string::npos)
        return calRingPosition(rs);

    if (surfaceType_.find("disk") != string::npos)
        return calDiskPosition(rs);

    if (surfaceType_.find("vol") != string::npos)
        return calVolPosition(rs);

    if (surfaceType_.find("sur") != string::npos)
        return calSurPosition(rs);

    Vec3<double> v(0, 0, 0);
    return v;
}

// vim:foldmethod=syntax:foldlevel=0
