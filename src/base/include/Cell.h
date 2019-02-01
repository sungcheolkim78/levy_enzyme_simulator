// Cell.h
// collection of walkers
//
// author: sungcheolkim @ IBM
// date: 20160202
// date: 20170901 - modify output log
// date: 20170906 - include multiple clouds
//

#ifndef CELL_H
#define CELL_H

#include "Cloud.hpp"
#include "EnzymeWalker.h"
#include "CellGeo.h"
#include <vector>

using namespace std;

class Cell {

private:
//////////////////////////////////////////////////////
// enzyme statistics
float ActiveEfficiency_;
float WallEfficiency_;
float ActiveMFPath_;
float WallMFPath_;
float maxDuration_;
float max_age_;
float max_substrate_conc_;

unsigned int TotalActive_;
unsigned int TotalWall_;

CellGeo cg_;
Cloud<EnzymeWalker> enzymeCloud_;
Cloud<EnzymeWalker> substrateCloud_;

public:
Cell(ParameterReader& pr) :
  enzymeCloud_{pr},
  substrateCloud_{pr},
  ActiveEfficiency_{0.0},
  WallEfficiency_{0.0},
  max_age_{0.0},
  max_substrate_conc_{0.0},
  cg_{pr, r_} {
  cout << "[Cell] is initialized." << endl;

  injectEnzyme(pr);
  }
~Cell() {
}

void injectEnzyme(ParameterReader& pr);
void evolve(double dt);
void write();

void updateEff();
void info(ParameterReader& pr);
};
#endif

// vim: foldmethod=syntax:foldlevel=1
