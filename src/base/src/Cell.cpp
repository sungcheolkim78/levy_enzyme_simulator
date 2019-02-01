////////////////////////////////////////////////////////////////////////////
// Cell.cpp
// implementation of cell class - collection of enzymeWalkers, cell geometry
// and active sites
//
// author: sungcheolkim @ IBM
// date: 20160202 version: 1.0.0
// date: 20160617 version: 1.1.0 update: change names

#include "../include/Cell.h"

using namespace std;

void Cell::injectEnzyme(ParameterReader& pr)
{
  Vec3<double> initialposition(0,0,0);
  for(int i=0; i < pr.particleNumber(); i++)
  {
      if (pr.isCenter())
        initialposition = cg_.calCenterPosition();
      else
        initialposition = cg_.calRandomPosition();

      EnzymeWalker w(initialposition, pr);
      addWalker(w);
      if (pr.debug()) {
        cout << "... initial position: ";
        initialposition.display();
      }
  }
}

void Cell::evolve(double dt)
{
  enzymeCloud_.moveWalker(dt);
  substrateCloud_.moveWalker(0.0);
}

void Cell::write()
{
  enzymeCloud_.writeWalker();
  substrateCloud_.writeWalker();
}

void Cell::updateEff()
{
    float af = 0.0, wf = 0.0;
    float amfp = 0.0, wmfp = 0.0;
    unsigned int noCountA = 0, noCountW = 0;
    TotalActive_ = 0; TotalWall_ = 0;
    double dur = 0.0;

    for(size_t i=0; i<enzymeCloud_.size(); i++)
    {
      auto ew = enzymeCloud_.getWalker(i);
      af += ew.getActiveFrequency();
      wf += ew.getWallFrequency();
      dur += ew.getDuration();
      max_age_ = (max_age_ > ew.age())? max_age_:ew.age();
      max_substrate_conc_ = (max_substrate_conc_ > ew.getMaxSubConc())? max_substrate_conc_:ew.getMaxSubConc();

      TotalActive_ += ew.hitActive();
      TotalWall_ += ew.hitWall();

      if (ew.hitActive() > 0)
          amfp += ew.trace()/static_cast<float>(ew.hitActive());
      else
          noCountA++;
      if (ew.hitWall() > 0)
          wmfp += ew.trace()/static_cast<float>(ew.hitWall());
      else
          noCountW++;
    }
    ActiveEfficiency_ =  af;
    WallEfficiency_ = wf/static_cast<float>(enzymeCloud_.size());
    ActiveMFPath_ = amfp/static_cast<float>(enzymeCloud_.size()-noCountA);
    WallMFPath_ = wmfp/static_cast<float>(enzymeCloud_.size()-noCountW);
    maxDuration_ = dur/static_cast<float>(enzymeCloud_.size()-noCountA);
}

void Cell::info(ParameterReader& pr)
{
    updateEff();
    cout << "... Generated Product Number: " << TotalActive_ << endl;
    cout << "... Reaction Rate [Hz]: " << ActiveEfficiency_ << endl;
    cout << "... Enzyme Mean Free Path [um]: " << ActiveMFPath_ << endl;
    cout << "... Wall Hits: " << TotalWall_ << endl;
    cout << "... Wall Hit Rate [Hz]: " << WallEfficiency_ << endl;
    cout << "... Enzyme Mean Free Path to Wall [um]: " << WallMFPath_ << endl;
    cout << "... Max Residence Time [s]: " << maxDuration_ << endl;
    cout << "... Max Age [s]: " << max_age_ << endl;
    cout << "... Max Captured Substrate Concentration [uM]: " << max_substrate_conc_ << endl;

    fstream file;
    file.open("cloud_log.txt", ios::out|ios::app);
    file << TotalActive_ << " " << ActiveEfficiency_ << " "
        << ActiveMFPath_ << " " << TotalWall_ << " "
        << WallEfficiency_ << " " << WallMFPath_ << " "
        << maxDuration_ << " " << max_age_ << " "
        << max_substrate_conc_ << endl;
}

void CellGeo::buildActiveSite()
{
  cout << "... create " << ActiveSiteNumber_ << " substrates " << endl;
    if(boolReadActiveSite_) {
        cout << "... load substrate positions : active_site.pt" << endl;
        ifstream infile;
        infile.open("active_site.pt");
        if (!infile.good()) {
            cerr << "... no active_site.pt file" << endl;
            exit(1);
        }

        for(int i=0; i<ActiveSiteNumber_; i++)
        {
            float x,y,z;
            infile >> x >> y >> z;
            Vec3<double> a(x,y,z);
            ActiveSiteList_[i] = a;
        }
    }
    else {
        ofstream outfile;
        outfile.open("active_site.pt");

        for(int i=0; i<ActiveSiteNumber_; i++)
        {
            ActiveSiteList_[i] = calActiveSite();

            outfile << ActiveSiteList_[i].getX() << " " << ActiveSiteList_[i].getY() << " " << ActiveSiteList_[i].getZ() << endl;
        }
        cout << "... save substrate positions : active_site.pt" << endl;
    }
}
// vim:foldmethod=syntax:foldlevel=0
