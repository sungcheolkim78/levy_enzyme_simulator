// CloudCell.h
// collection of walkers in cell
//
// author: sungcheolkim @ IBM
// date: 20170910 - initial version
// date: 20171005 - cell method (PID)
// date: 20171006 - implement non stop movement
// date: 20171009 - implement sweep algorithm
// date: 20171012 - injection method

#ifndef CLOUDCELL_H
#define CLOUDCELL_H

#include "CloudBase.hpp"
#include "Vec3.hpp"
#include "ParameterReader.h"
#include <gsl/gsl_const_num.h>

using namespace std;

class CloudCell: public CloudBase {
public:
  // overloading functions
  void moveWalker(double dt);
  void info(Log* log_);
  void setSubstrateCloud(Cloud* sc);

  // member functions
  double getTimeForSubstrate(Vec3<double> p, Vec3<double> dr, double dt);
  size_t countSubstrate(Vec3<double> p, Vec3<double> dr);
  double getDuration(int count, Walker* w);

  // constructor
  CloudCell(ParameterReader& pr, string cloudID):
    CloudBase{pr, cloudID},
    hitSubstrate_(0),
    focusConc_(0.0),
    substrateOn_(false),
    reactionOn_(false),
    writeCount_(false)
  {
    cout << blu << "[Cell Cloud (" << cloudID << ")] is initialized" << def << endl;

    substrateOn_ = pr.boolRead(cloudID + " Substrate On", "True");

    if (substrateOn_) {
      sightDistance_ = pr.doubleRead(cloudID + " Sight Distance", "5")/1000.0;
      reactionOn_ = pr.boolRead(cloudID + " Reaction On", "True");
      focusConc_ = pr.doubleRead(cloudID + " Focus Concentration", "1.0");
      substrateConstant_ = pr.boolRead(cloudID+" Substrate Constant", "True");
      if (reactionOn_) {
        Km_ = pr.doubleRead(cloudID + " Km", "8.9");
        Kcat_ = pr.doubleRead(cloudID + " Kcat", "6.3");
      }
      writeCount_ = pr.boolRead(cloudID + " Write Count", "True");
      if(writeCount_) {
        string tmp = pr.simfilename();
        if(tmp.find(".par") != string::npos)
          saveCountName_ = tmp.substr(0, tmp.find(".par")) + "_count.txt";
        else if(tmp.find(".txt") != string::npos)
          saveCountName_ = tmp.substr(0, tmp.find(".txt")) + "_count.txt";
        else
          saveCountName_ = tmp + "_count.txt";
      }
    }
  }
  virtual ~CloudCell() { };

  // inline functions
  inline int hitSubstrate() { return hitSubstrate_; }
  inline void hitSubstrate(int h) { hitSubstrate_ = h; }
  inline double sightDistance() { return sightDistance_; }
  inline void sightDistance(double s) { sightDistance_ = s; }
  inline double Km() { return Km_; }
  inline void Km(double k) { Km_ = k; }
  inline double Kcat() { return Kcat_; }
  inline void Kcat(double k) { Kcat_ = k; }

protected:
  size_t hitSubstrate_;
  vector<double> productConcentration_;
  vector<double> freeTimeArray_;
  vector<double> freeLengthArray_;
  double sightDistance_;
  double focusConc_;
  double Km_;
  double Kcat_;
  double searchTime_;
  double reactionTime_;
  double meanVel_;
  string saveCountName_;

  bool substrateOn_;
  bool substrateConstant_;
  bool reactionOn_;
  bool writeCount_;
  Cloud* substrateCloudPtr_;

private:

};

void CloudCell::setSubstrateCloud(Cloud* sc) {
  // cases
  // 1. substrate on, reaction off
  // 2. substrate on, reaction on
  //    2.1 substrate on, diffusion cases
  //    2.2 substrate on, cluster cases
  // 3. substrate off

  substrateCloudPtr_ = sc;
  cout << "... Reaction with " << substrateCloudPtr_->cloudID() << "(" << substrateCloudPtr_->walkerType() << ")" << endl;

  // calculate key parameters
  double csa = M_PI*sightDistance()*sightDistance();
  cout << "... cal Reaction Cross-section Area: " << gre << csa << def << " [um2]" << endl;
  double mfp = 1.0e21/(GSL_CONST_NUM_AVOGADRO*sc->concentration()*csa);
  cout << "... cal Mean Free Path: " << gre << mfp << def << " [um]" << endl;
  meanVel_ = sqrt(GSL_CONST_MKSA_BOLTZMANN*temperature()/(*this)[0]->mass());
  cout << "... cal Thermal Mean Velocity: " << gre << meanVel_ << def << " [um/s]" << endl;
  searchTime_ = mfp/meanVel_;
  cout << "... cal Mean Diffusion Time: " << gre << searchTime_ << def << " [s]" << endl;

  if (reactionOn_) {
    // diffusion case
    if (focusConc_ == 0.0) {
      reactionTime_ = (sc->concentration()+concentration()+Km())/(Kcat()*sc->concentration());
      cout << "... cal MM Reaction Time: " << gre << reactionTime_ << def << " [s]" << endl;
      if (reactionTime_ < searchTime_) {
        cout << "... reaction time is less than diffusion time. Adjust sight distance";
        exit(1);
      }
      cout << "... cal Residence Time: " << gre << reactionTime_ - searchTime_ << def << " [s]" << endl;
      cout << "... cal MM rate: " << gre << (Kcat()*sc->cellConcentration()*cellConcentration())/(sc->cellConcentration()+cellConcentration()+Km()) << def << " [uM/s] " << endl;
    } else {
      cout << "... cal MM rate: " << gre << (Kcat()*sc->cellConcentration()*focusConc_)/(sc->cellConcentration()+focusConc_+Km()) << def << " [uM/s] " << endl;
    }
  } else {
    reactionTime_ = searchTime_;
    cout << "... no residence time " << endl;
  }
}

void CloudCell::info(Log* log_) {
  // if substrate cloud or fixed cloud
  if ((D() == 0.0) or (cloudID()=="Substrate"))
    return;

  // collect informations from walkers
  size_t totalWallHit{0};
  double age = (*this)[0]->age();
  double meanFreeTime = 0;
  double meanFreeLength = 0;

  // linear slope for mean velocity
  double rate = productConcentration_.back()/age;
  // local slope for instanteous velocity
  double inst_rate = 0;
  if (productConcentration_.size() > 201)
    inst_rate = (productConcentration_.back() - productConcentration_[productConcentration_.size()-200])/(200.0*dt());

  cout << "Time: " << age << " [s]" << endl;
  cout << "Total Product: " << hitSubstrate() << endl;
  cout << "Product Concentration: " << productConcentration_.back() << " [uM]" << endl;
  cout << "Product Rate: " << red << rate << def << " [uM/s] (" << red << inst_rate << def << ") [uM/s]" << endl;

  for (auto w : wlist_) { totalWallHit += w->wallHit(); }
  cout << "Wall Hit: " << totalWallHit << endl;
  auto wpressure = 1e14*2.0*(*this)[0]->mass()*meanVel_*(double)totalWallHit/(sf_->surfaceArea()*age);
  cout << "Wall Hit Pressure: " << wpressure << " [mbar]" << endl;

  for (auto ft : freeTimeArray_) meanFreeTime += ft;
  for (auto fl : freeLengthArray_) meanFreeLength += fl;
  if (freeTimeArray_.size() > 0) {
    meanFreeTime /= (double)freeTimeArray_.size();
    meanFreeLength /= (double)freeLengthArray_.size();
  }
  cout << "Mean Free Time: " << meanFreeTime << " [s] (" << freeTimeArray_.size() << ")" << endl;
  cout << "Mean Free Length: " << meanFreeLength << " [um] (" << freeLengthArray_.size() << ")" << endl;

  // features on save file
  string log_msg = to_string(age)+" "+
                   to_string(focusConc_)+" "+
                   to_string((*this)[0]->r())+" "+
                   to_string(substrateCloudPtr_->cellConcentration())+" "+
                   to_string(hitSubstrate())+" "+
                   to_string(productConcentration_.back())+" "+
                   to_string(rate)+" "+
                   to_string(totalWallHit)+" "+
                   to_string(wpressure)+" "+
                   to_string(meanFreeTime)+" "+
                   to_string(meanFreeLength)+" "+
                   to_string(freeLengthArray_.size())+" "+
                   infoString();
  log_->write(log_msg);

  // write concentration infomation
  if(writeCount_) {
   fstream f;
   f.open(saveCountName_, ios::out|ios::app);
   if(substrateOn_)
     f << age << " " << concentration() << " " << substrateCloudPtr_->concentration() << " " << productConcentration_.back() << endl;
   else
     f << age << " " << concentration() << " " << productConcentration_.back() << endl;
   f.close();
  }
}

void CloudCell::moveWalker(double dt) {

  // move walkers for total dt time
  for (auto w : wlist_) {
    // time(age) shift
    w->addAge(dt);

    // fixed position clouds
    if (D() == 0.0) continue;

    Vec3<double> dr;
    int subcycleIteration = 0;
    double pt_ = dt;   // remaining time - keep decreasing

    // start subcycle
    while (pt_ > 0) {
      subcycleIteration++;

      // Case1: enzyme full stay
      if (w->duration() > pt_) {
        if (debug_)
          cout << red << "... subcycle[" << subcycleIteration << "] stay - pt_: " << pt_ << " duration_: " << w->duration() << def << endl;
        w->subDuration(pt_);
        pt_ = 0.0;
        w->step(Vec3<double>{0.0, 0.0, 0.0});
        continue;
      }

      // Case2: enzyme partial stay but start to move within dt
      if ((w->duration() < pt_) and (w->duration() > 0.0)) {
        if (debug_)
          cout << red << "... subcycle[" << subcycleIteration << "] partial stay - pt_: " << pt_ << " duration_: " << w->duration() << def << endl;
        pt_ -= w->duration();
        w->duration(0.0);
        continue;
      }

      // enzyme move
      if (w->duration() == 0.0) {
        dr = getStep(pt_);

        // check distance to wall and other substrate
        double tt_w = sf_->getTimeForSurface(w->position(), dr);
        size_t substrate_number = 0;

        // Case3: wall hit before substrate hit
        if ((tt_w < 1.0) and (tt_w >= 0.0)) {
          // substrate collision count with wall hit
          Vec3<double> dr0 = dr;
          dr = sf_->calNewStep(w->position(), dr, tt_w, 0);
          if (substrateOn_) {
            substrate_number += countSubstrate(w->position(), dr0*tt_w);
            substrate_number += countSubstrate(w->position()+dr0*tt_w, dr - dr0*tt_w);
          }
          if (debug_)
            cout << red << "... subcycle[" << subcycleIteration << "] found wall - pt_: " << pt_*tt_w << def << endl;
          w->addWallHit(1);
          //substrate_number += countSubstrate(w, dr*tt_w);
        } else {
          // substrate collision count without wall hit
          if (substrateOn_) substrate_number += countSubstrate(w->position(), dr);
        }

        // Case4: freely move
        if (debug_)
          cout << red << "... subcycle[" << subcycleIteration << "] move - pt_: " << pt_ << " duration_: " << w->duration() << def << endl;
        //  update pid list
        auto p_pid = w->pid();
        w->step(dr);
        auto n_pid = w->pid();
        if (p_pid != n_pid) {
          pidList_[p_pid].erase(w->tid());
          pidList_[n_pid].insert(w->tid());
        }
        pt_ = 0.0;

        // Case5: substrate hit before wall hit
        if (substrate_number > 0) {
          // calculate duration based on substrate count
          w->duration(getDuration(substrate_number, w));
          w->addSubstrateHit(substrate_number);

          if (debug_)
            cout << red << "... subcycle[" << subcycleIteration << "] (" << w->pid() << ") found " << substrate_number << " substrates at tt_w= " << tt_w << " with duration = " << w->duration() << " [s] " << def << endl;

          // calculate free time before the reaction
          if (w->lastHitAge() > 0.0) {
            double ft = w->age() - w->lastHitAge();
            freeTimeArray_.push_back(ft);
            freeLengthArray_.push_back((w->position() - w->lastHitPosition()).mag());
          }
          w->lastHitAge(w->age());
          w->lastHitPosition(w->position());

        }
      }
    }
  }
  // keep counting product concentration
  // volume [um3], concentration [uM], 1 [uL] = 1e+3 [m3]
  double pc = (double)(hitSubstrate_)/(sf_->volume()*GSL_CONST_NUM_AVOGADRO*1e-21);
  productConcentration_.push_back(pc);
}

double CloudCell::getDuration(int count, Walker* w) {
    if(count==0) { return 0.0; }

    if(!reactionOn_) { return 0.0; }

    // for diffusion case (using diffusion-limited on rate)
    float residence_time = reactionTime_ - searchTime_;

    // for cluster reaction case
    if (focusConc_>0.0) {
      // using Michaelis-Menten equation (constant)
      double substrate_conc = count/(w->volume()*1e-18*GSL_CONST_NUM_AVOGADRO*1e-3);  // [uM]
      //substrate_conc = substrateCloudPtr_->concentration();
      double clusterConc = (focusConc_*sf_->volume()/w->volume()); // [uM]
      residence_time = (Km_ + substrate_conc)/(Kcat_*clusterConc);
      //residence_time = Km_/(Kcat_*clusterConc);

      if (debug_) {
        cout << "... [" << this->cloudID_ << "][" << w->tid() << "] t=" << w->age() << " capture: " << count << " residence_time: " << residence_time << " [s] " << endl;
        cout << "    substrate concentration [uM]: " << substrate_conc << " cluster concentration [uM]: " << clusterConc << endl;
      }
    }

    return residence_time;
}

size_t CloudCell::countSubstrate(Vec3<double> p, Vec3<double> dr) {
  vector<size_t> sublist;

  // check for all substrates
  auto sslist = substrateCloudPtr_->getLocationList(p, dr);
  for(auto i : sslist) {
    // count all substrate around current position
    Vec3<double> new_position = p + dr;
    Vec3<double> sp = (*substrateCloudPtr_)[i]->position();
    double pr = sightDistance_/dr.mag();
    double t = Vec3<double>::dotProduct(new_position-sp, dr)/dr.mag2();
    if ((t>0.0) and (t<=1.0)) {
      if((new_position*t+p*(1.0-t)-sp).mag() <= sightDistance_) {
        sublist.push_back(i);
        // cout << "... found [" << i << "] in " << sslist.size() << endl;
      }
    } else if ((t>0.0) and (t <= 1.0+pr)) {
      if((sp - new_position).mag() <= sightDistance_) {
        sublist.push_back(i);
      // cout << "... found [" << i << "] in " << sslist.size() << endl;
      }
    }
  }

  if (sublist.size() == 0) return 0;

  for(auto subidx : sublist) {
    if (!substrateConstant_) {
      // delete particle
      if (debug_) cerr << "... remove substrate [" << subidx << "]" << endl;
      substrateCloudPtr_->removeWalker(subidx);
    } else {
      if (focusConc_ == 0.0) {
        // make new active site
        Vec3<double> temp = (substrateCloudPtr_->sf())->calRandomPosition(rs_);
        (*substrateCloudPtr_)[subidx]->position(temp);
        (*substrateCloudPtr_)[subidx]->pid((*substrateCloudPtr_)[subidx]->calPID());
      }
      // let points stay in case of cluster
    }
  }
  // update enzyme info
  hitSubstrate_ += sublist.size();

  return sublist.size();
}

double CloudCell::getTimeForSubstrate(Vec3<double> p, Vec3<double> dr, double dt) {
  if (!substrateOn_)
    return 2.0;

  auto new_position = p+dr;
  double min_t = 2.0;
  double max_t = 0.0;

  auto sslist = substrateCloudPtr_->getLocationList(p, dr);
  for(auto i : sslist) {
    Vec3<double> aS{(*substrateCloudPtr_)[i]->position()};

    // find collision condition for trajectory
    double t = Vec3<double>::dotProduct(new_position-aS, dr)/dr.mag2();
    if ((t>0.0) and (t<=1.0)) {
      if((new_position*t+p*(1.0-t)-aS).mag() <= sightDistance_) {
        if (debug_)
          cout << "... stop at substrate[" << i << "] (" << (*substrateCloudPtr_)[i]->pid() << ") p=" << p << " t = " << t << endl;
        if (min_t > t) min_t = t;
        if (max_t < t) max_t = t;
      }
    }
  }

  // cluster case: find longest distance when substrate is found
  if (focusConc_ > 0.0) {
    // no substrate
    if (max_t == 0.0) return 2.0;
    else return max_t;
  }
  else return min_t;
}

#endif
// vim:foldmethod=syntax:foldlevel=0