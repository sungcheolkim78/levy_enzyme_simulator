// Cloud.h - abstract class for Cloud
// collection of walkers
//
// author: sungcheolkim @ IBM
// date: 20160202
// date: 2017/09/06 - generalize for multiple clouds
// date: 2017/09/12 - using abstract class

#ifndef CLOUD_H
#define CLOUD_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <list>
#include <set>

#include "Vec3.hpp"
#include "Log.hpp"
#include "ParameterReader.h"
#include "Surfaces.hpp"
#include "Walker.h"
#include "WalkerBase.h"

using namespace std;

class Cloud {

public:
  // virtual functions
  virtual void setProperties(ParameterReader& pr) = 0;
  virtual void injectWalkers(ParameterReader& pr) = 0;
  virtual void moveWalker(double dt) = 0;
  virtual void info(Log* log_) = 0;
  virtual void setSubstrateCloud(Cloud* sc) = 0;
  virtual double concentration() = 0;

  // member functions
  void addWalker(Walker* w);
  void removeWalker(size_t tid);
  unsigned int size() { return wlist_.size(); }
  Walker* operator[](int i) {
    if (i<0 || size()<=i) throw out_of_range{"Cloud::operator[]"};
    return wlist_[i];
  };

  void writeWalker();
  Vec3<double> getStep(double dt);
  void writeHeader(string fn);
  set<size_t> getLocationList(Vec3<double> p, Vec3<double> dr);

  // inline functions
  string cloudID() { return cloudID_; }
  void cloudID(string cID) { cloudID_ = cID; }
  string savefilename() { return savefilename_; }
  void savefilename(string fname) { savefilename_ = fname; }
  gsl_rng* rs() { return rs_; }
  void rs(gsl_rng* rs) { rs_ = rs; }
  double D() { return D_; }
  void D(double dc) { D_ = dc; }
  double alpha() { return alpha_; }
  void alpha(double a) { alpha_ = a; }
  string surfaceShape() { return surfaceShape_; }
  void surfaceShape(string s) { surfaceShape_ = s; }
  string walkerType() { return walkerType_; }
  void walkerType(string s) { walkerType_ = s; }
  Surfaces* sf() { return sf_; }
  void sf(Surfaces* sf) { sf_ = sf; }
  double dt() { return dt_; }
  void dt(double t) { dt_ = t; }

protected:
  // cloud related information
  vector<Walker*> wlist_;
  Surfaces* sf_;

  string cloudID_;
  string savefilename_;
  string walkerType_;
  string surfaceShape_;

  gsl_rng* rs_;

  // particle related information
  double D_;
  double alpha_;
  double dt_;

  // virtual cloud for particle particle interaction
  vector<set<size_t>> pidList_;
};

void Cloud::writeHeader(string fn) {
  cout << "... prepare track file header: " << fn << endl;
  ofstream outfile;
  outfile.open(fn.c_str());
  outfile << "t x y z r duration tid pid" << endl;
  outfile.close();
}

void Cloud::addWalker(Walker* w) {
  wlist_.push_back(w);
  pidList_[w->pid()].insert(w->tid());
}

void Cloud::removeWalker(size_t tid) {
  if (size() <= tid) {
    cout << "... no walker[" << tid << "] exists." << endl;
    return;
  }

  using std::swap;
  swap(wlist_[tid], wlist_.back());
  wlist_.pop_back();
}

void Cloud::writeWalker() {
  for(auto w : wlist_) w->write(savefilename_);
}

Vec3<double> Cloud::getStep(double dt) {
  Vec3<double> dr{0,0,0};
  if (D_ == 0.0)
    return dr;

  /* original method - high peak near 0
  float random_step = sqrt(2.0*D_*dt)*gsl_ran_levy(rs_, 1.0, alpha_);

  double ux, uy, uz;
  gsl_ran_dir_3d(rs_, &ux, &uy, &uz);

  dr.set(random_step*ux, random_step*uy, random_step*uz);
  */

  // levy distribution becomes gaussian distribution with sigma=\sqrt{2}*c
  double ux = sqrt(D_*dt)*gsl_ran_levy(rs_, 1.0, alpha_);
  double uy = sqrt(D_*dt)*gsl_ran_levy(rs_, 1.0, alpha_);
  double uz = sqrt(D_*dt)*gsl_ran_levy(rs_, 1.0, alpha_);

  dr.set(ux, uy, uz);

  return dr;
}

set<size_t> Cloud::getLocationList(Vec3<double> p, Vec3<double> dr) {
  size_t initial_pid = wlist_[0]->calPID(p);
  size_t final_pid = wlist_[0]->calPID(p+dr);

  // check in same partition
  if (initial_pid == final_pid)
    return pidList_[final_pid];

  set<size_t> l{initial_pid, final_pid};

  // find other partition in case of middle point is in different pid
  size_t middle_pid = wlist_[0]->calPID(p+dr*0.5);
  if (l.find(middle_pid) == l.end()) {
    l.insert(middle_pid);
    middle_pid = wlist_[0]->calPID(p+dr*0.25);
    if (l.find(middle_pid) == l.end()) {
      l.insert(middle_pid);
      middle_pid = wlist_[0]->calPID(p+dr*0.75);
      if (l.find(middle_pid) == l.end())
        l.insert(middle_pid);
    }
  }

  // make new set for search
  //cout << wlist_.size() << ")" << endl;
  set<size_t> a;
  for(auto pid : l)
    for(auto element : pidList_[pid])
      a.insert(element);

  return a;
}

#endif

// vim: foldmethod=syntax
