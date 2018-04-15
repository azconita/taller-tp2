#ifndef SCREW_CLASSIFIER_H
#define SCREW_CLASSIFIER_H

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <queue>
#include "packager.h"
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

#endif
