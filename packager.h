#ifndef SCREW_PACKAGER_H
#define SCREW_PACKAGER_H

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <utility>
#include "configuration.h"
#include "screwblock.h"

class ScrewClassifier {
  private:
  std::ifstream infile;
  std::string name;
  std::queue<ScrewBlock> *classified;

  public:
  ScrewClassifier (const char* filename, std::queue<ScrewBlock> *classified);
  ScrewClassifier(ScrewClassifier &&other) : infile(std::move(other.infile)), name(other.name), classified(other.classified) {}
  void print_content();
  void classify();
  ~ScrewClassifier();

};

class ScrewsUnpackaged {
private:
  int total = 0;
  std::vector<int> widths;
  int limit = 0;
  int type;
public:
  ScrewsUnpackaged (int type, int limit) : type(type), limit(limit) {}
  virtual ~ScrewsUnpackaged () {
    this->widths.clear();
  }
  int get_type() {
    return this->type;
  }
  int get_size() {
    return this->widths.size();
  }
  int make_package(int quantity, int width, int limit, std::string btype) {
    int left = quantity;
    int packages = 0;
    for (size_t i = 0; i < quantity; i++) {

      if (this->widths.size() == limit) {
        int m = this->get_median(limit);
        std::cout << "Paquete listo: " << limit << " " << btype<< " " << " mediana: " << m << '\n';
        ++packages;
        this->widths.clear();
      }
      this->widths.push_back(width);
    }
  }
  int get_median(int limit) {
    return this->widths[limit / 2];
  }
};

class ScrewPackager {
private:
  ScrewConfiguration config;
  std::queue<ScrewBlock> classified;
  std::vector<ScrewClassifier> classificators;
  int total = 0;
  std::map<int, ScrewsUnpackaged> screws_unpackaged;
public:
  ScrewPackager (const char * cfg_file) : config(cfg_file){}
  virtual ~ScrewPackager ();
  void connect_to_classificators(const int total, const char *file_classif[]);
  void start();
  void make_packages();
};

#endif
