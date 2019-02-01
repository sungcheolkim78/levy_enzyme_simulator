// ParameterReader.h
// read parameters from par file
//
// author: sungcheol kim @ IBM
// date: 20160616 version: 1.0.1 update: add comments
// date: 20160617 version: 1.0.2 update: separate read file pattern
// date: 2017-09-18 version: 2.0.0 update: add more types

#ifndef PARAMETERREADER_H
#define PARAMETERREADER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Vec3.hpp"

using namespace std;

class ParameterReader {

  private:
    string simfilename_;
    string printbuffer_;
    vector<string> lines_;

  public:
  ParameterReader(string filename) {
    simfilename_ = filename;

    cout << "[ParameterReader] is initialized." << endl;
    cout << "... parameter file : " << simfilename_ << endl;

    printbuffer_ = "...";
    readfromtxt(filename);
  }
  ParameterReader() {
    ParameterReader("test.par");
  }
  virtual ~ParameterReader() {
    save(simfilename_);
  };

  void readfromtxt(string fn);
  void info();
  void save(string fn);

  // basic read function
  string stringRead(string name, string defvalue, bool verbose=true, string selector=":");
  inline double doubleRead(string name, string defvalue, bool verbose=true, string selector=":") {
    string::size_type sz; return stod(stringRead(name, defvalue, verbose, selector), &sz);
  }
  inline int intRead(string name, string defvalue, bool verbose=true, string selector=":") {
    string::size_type sz; return stoi(stringRead(name, defvalue, verbose, selector), &sz);
  }
  bool boolRead(string name, string defvalue, bool verbose=true, string selector=":");

  Vec3<double> vec3Read(string name, string defvalue, bool verbose=true, string selector=":");
  vector<string> arrayRead(string name, string defvalue, bool verbose=true, string selector=":");

  bool checkName(string name, string selector=":");

  inline string simfilename() { return simfilename_; }
};

#endif

// vim:foldmethod=syntax:foldlevel=1
