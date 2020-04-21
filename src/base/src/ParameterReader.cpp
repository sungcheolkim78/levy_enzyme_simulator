// ParameterReader.cpp
//
// author: sung cheol kim @IBM
// date: 20160616 version: 1.0.0

#include "../include/ParameterReader.h"

using namespace std;

void ParameterReader::info() {
    cout << string(80, '-') << endl;
    cout << printbuffer_ << endl;
    cout << string(80, '-') << endl;
}

void ParameterReader::readfromtxt(string fn) {
  ifstream infile(fn.c_str());
  if (!infile.good()) {
    cerr << "... " << fn << " does not exist." << endl;
    exit(1);
  }
  for (string line; getline(infile, line); )
    lines_.push_back(line);
}

void ParameterReader::save(string fn) {
  ofstream outfile(fn.c_str());
  for (int n = 0; n < lines_.size(); n++) {
    outfile << lines_[n] << endl;
  }
}

bool ParameterReader::checkName(string name, string selector) {
    for (size_t i=0; i<lines_.size(); i++) {
      // check comment lines
      if (lines_[i].find("#") == 0) continue;

      auto found = lines_[i].find(name);
      //cout << "[" << i << "] at found " << found << " " << lines_[i] << endl;
      if (found != string::npos)
          return true;
    }
    cout << "... no found " << name << endl;

    return false;
}

string ParameterReader::stringRead(string name, string defvalue, bool verbose, string selector) {
    string line;
    string result;
    string red = "\033[0;31m";
    string def = "\033[0m";

    for (size_t i=0; i<lines_.size(); i++) {
      // check comment lines
      if (lines_[i].find("#") == 0) continue;

      // find name id
      auto found = lines_[i].find(name);
      if (found != string::npos) {
        //cout << "[" << i << "] find: " << name << endl;
        // find selector
        auto found_selector = lines_[i].find(selector, found+1);
        if (found_selector != string::npos) {
          result = lines_[i].substr(found_selector+1);
          // print items if it appear first time
          if (printbuffer_.find(name) == string::npos) {
            if (verbose)
              cout << "... set " << name << " " << selector << " " << red << result << def << endl;
            printbuffer_.append("/"+name);
          }
          // delete empty spaces
          result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
          return result;
        }
        else {
          // not find selector character
         cerr << "... " << selector << " is not found for [ " << name << " ]" << endl;
         exit(1);
        }
      }
    }
    // not find item name
    if (defvalue == " ") {
      cerr << "... no parameter " << name << endl;
      save(simfilename_);
      exit(1);
    } else {
      string line; 
      line = name + selector + defvalue;
      lines_.push_back(line);
      cerr << "... add: " << line << endl;
      return defvalue;
    }
}

bool ParameterReader::boolRead(string name, string defvalue, bool verbose, string selector) {
    string str = stringRead(name, defvalue, verbose, selector);

    if (str.find("Yes") != string::npos) return true;
    if (str.find("No") != string::npos) return false;
    if (str.find("True") != string::npos) return true;
    if (str.find("False") != string::npos) return false;

    cerr << "... [ " << name << " ] should be [Yes|No|True|False]" << endl;
    exit(1);
}

vector<string> ParameterReader::arrayRead(string name, string defvalue, bool verbose, string selector) {
  string res = stringRead(name, defvalue, false, selector);
  vector<string> result;
  string red = "\033[0;31m";
  string def = "\033[0m";

  size_t findidx = res.find("(");
  if (findidx == string::npos) { cerr << "... [Err] array format: (item1, item2, item3)" << endl; exit(1); }
  res = res.substr(findidx+1);

  bool loop_flag = true;
  do {
    findidx = res.find(",");
    if (findidx != string::npos) {
      string temp = res.substr(0, findidx);
      temp.erase(0, temp.find_first_not_of(" \t\n\r\f\v"));
      result.push_back(temp);
      res = res.substr(findidx+1);
    } else {
      findidx = res.find(")");
      if (findidx != string::npos) {
        string temp = res.substr(0, findidx);
        temp.erase(0, temp.find_first_not_of(" \t\n\r\f\v"));
        result.push_back(temp);
        loop_flag = false;
      }
    }
  } while (loop_flag);

  if (verbose) {
    cout << "... set " << name << "(" << result.size() << ") " << selector << " " << red;
    for (auto s : result) cout << s << ", ";
    cout << '\b' << " " << def << endl;
  }

  return result;
}

Vec3<double> ParameterReader::vec3Read(string name, string defvalue, bool verbose, string selector) {
  double x, y, z;
  string res = stringRead(name, defvalue, verbose, selector);
  string::size_type sz;

  size_t findidx = res.find("(");
  if (findidx == string::npos) { cerr << "... [Err] vector format: (0, 0, 0)" << endl; exit(1); }
  res = res.substr(findidx+1);

  size_t findidx2 = res.find(",");
  if (findidx2 == string::npos) { cerr << "... [Err] vector format: (0, 0, 0)" << endl; exit(1); }
  x = stod(res.substr(0, findidx2), &sz);
  res = res.substr(findidx2+1);

  findidx2 = res.find(",");
  if (findidx2 == string::npos) { cerr << "... [Err] vector format: (0, 0, 0)" << endl; exit(1); }
  y = stod(res.substr(0, findidx2), &sz);
  res = res.substr(findidx2+1);

  findidx2 = res.find(")");
  if (findidx2 == string::npos) { cerr << "... [Err] vector format: (0, 0, 0)" << endl; exit(1); }
  z = stod(res.substr(0, findidx2), &sz);

  //cout << "(" << x << "," << y << "," << z << ")";
  return Vec3<double>(x, y, z);
}

// vim:foldmethod=syntax:foldlevel=0
