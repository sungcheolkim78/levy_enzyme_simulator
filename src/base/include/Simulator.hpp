// Simulator.hpp
// Top layer class for walker simulation
//
// author: Sung-Cheol Kim @ IBM
// date: 2017/09/07 - derived from cell.h
//

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Cloud.hpp"
#include "CloudBase.hpp"
#include "CloudCell.hpp"
#include "ParameterReader.h"
#include "progress_bar.hpp"
#include "Log.hpp"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_const_num.h>
#include <sys/time.h>

using namespace std;
using namespace std::chrono;

class Simulator {

  public:
    // member functions
    void injectClouds(ParameterReader& pr);
    void writeClouds();
    void evolveClouds();
    void run();
    void info();

    // constructor
    Simulator(ParameterReader& pr, Log* lg):
      log_(lg),
      internalTime_(0.0),
      internalItr_(1),
      cloudCount_(0) 
    {
      cout << blu << "[Simulator] is initialized." << def << endl;
      dt_ = pr.doubleRead("dt", "0.0001");
      iteration_ = pr.intRead("iteration", "1000");
      cloudNames_ = pr.arrayRead("species Name", "(Enzyme, Substrate)");
      cloudNumber_ = cloudNames_.size();

      saveTrace_ = pr.boolRead("save Trace", "True");
      saveCycle_ = pr.intRead("save Cycle", "1");
      infoCycle_ = pr.intRead("info Cycle", "100");
      saveStep_ = pr.boolRead("save Step", "False");
      saveCount_ = pr.boolRead("save Count", "False");

      showProg_ = pr.boolRead("show Progress", "True");
      debug_ = pr.boolRead("debug", "False");

      cout << "... prepare random variable" << endl;
      struct timeval tv;
      gettimeofday(&tv, 0);
      unsigned long int random_seed = tv.tv_sec+tv.tv_usec;

      gsl_rng_env_setup();
      T_ = gsl_rng_default;   // gsl_rng_mt19937
      // T_ = gsl_rng_ranlxs0;
      rs_ = gsl_rng_alloc(T_);
      gsl_rng_set(rs_, random_seed);

      cout << "... generator: " << gsl_rng_name(rs_) << endl;
      cout << "... seed: " << random_seed << endl;
    }

    virtual ~Simulator() {
      gsl_rng_free(rs_);
    }

    inline gsl_rng* rs() { return rs_; }
    float dt() { return dt_; }
    void dt(double t) { dt_ = t; }
    size_t iteration() { return iteration_; }
    void iteration(size_t itr) { iteration_ = itr; }

  protected:
    vector<Cloud*> cloudList_;
    Log* log_;

    float internalTime_;
    size_t internalItr_;
    size_t cloudCount_;

    float dt_;
    size_t iteration_;
    size_t cloudNumber_;
    vector<string> cloudNames_;
    bool saveTrace_;
    size_t saveCycle_;
    size_t infoCycle_;
    bool saveStep_;
    bool saveCount_;
    bool showProg_;
    bool debug_;

    // prepare random number seed ; once for all
    const gsl_rng_type* T_;
    gsl_rng* rs_;

  private:
};

void Simulator::run() {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  cout << "... cal Total Simulation Time : " << dt_*iteration_ << " [s]" << endl;
  ProgressBar *bar = new ProgressBar(iteration_);
  size_t infoItr;
  string msg = "";
  msg = "dt: " + to_string(dt_) + " iteration: " + to_string(iteration_) + " [Start]";
  log_->timestamp(msg);

  for (infoItr=0; infoItr<iteration_; infoItr++) {
      evolveClouds();
      writeClouds();

      if(showProg_ && (infoItr%infoCycle_) == 0) {
          high_resolution_clock::time_point t2 = high_resolution_clock::now();
          int sep = 60;
          auto runningSec = duration_cast<seconds>(t2 - t1).count();
          if (infoItr == 0) { sep = 62; } 
          cout << blu << "[#] = " << infoItr << " " << string(sep,'-') << " " << runningSec << " [sec] " << def << endl;
          info();
      }

      bar->Progressed(infoItr);
  }

  // print info for the last iteration nd running time
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto runningSec = duration_cast<seconds>(t2 - t1).count();
  auto runningMin = duration_cast<minutes>(t2 - t1).count();
  cout << blu << "[#] = " << iteration_ << " " << string(35,'-') << " running time: " << runningMin << " [mins] " << runningSec - 60*runningMin << " [secs]" << def << endl;
  info();
}

void Simulator::injectClouds(ParameterReader& pr) {
  // inject clouds
  for (auto cname : cloudNames_) {
    cout << gre << "... add " << cname << def << endl;
    CloudCell* c = new CloudCell{pr, cname};
    c->rs(rs_);
    c->dt(dt_);
    c->injectWalkers(pr);
    cloudList_.push_back(c);
    cloudCount_++;
  }
  // check reactions
  for (auto i=0; i<cloudNames_.size(); i++)
    if (pr.boolRead(cloudNames_[i]+" Substrate On", "False")) {
      string substrateName = pr.stringRead(cloudNames_[i]+" Substrate Name", "Substrate");
      for (auto j=0; j<cloudNames_.size(); j++)
        if (substrateName == cloudNames_[j])
          cloudList_[i]->setSubstrateCloud(cloudList_[j]);
    }
  // write initial positions
  writeClouds();
}

void Simulator::evolveClouds() {
  for(auto i=0; i<cloudCount_; i++) {
    cloudList_[i]->moveWalker(dt_);
  }

  internalTime_ += dt_;
  internalItr_++;
}

void Simulator::writeClouds() {
  if (saveTrace_) {
    if (internalItr_%saveCycle_ == 0)
      for (size_t i=0; i<cloudCount_; i++)
        cloudList_[i]->writeWalker();
  }
}

void Simulator::info() {
  for (size_t i=0; i<cloudCount_; i++)
    cloudList_[i]->info(log_);
}
#endif

// vim:foldmethod=syntax:foldlevel=0
