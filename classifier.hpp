#ifndef PARSER_CLASSIFIER_H
#define PARSER_CLASSIFIER_H

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <queue>


class ScrewBlock {
  private:
  int type = 0;
  int quantity = 0;
  short int width = 0;

  public:
  ScrewBlock(const unsigned char block[4]) {
    this->type = (int) ((block[0] & 0xF8) >> 3);
    this->quantity = (int) (((block[0] & 0x07) << 24 | block[1] << 16 |
                      block[2] << 8 | (block[3] & 0xe0)) >> 5);
    this->width = (short int) (block[3] & 0x1F);
  }
  ~ScrewBlock() {}
};

class ScrewClassifier {
  private:
  std::ifstream infile;
  std::string name;
  std::queue<ScrewBlock> classified;

  public:
  ScrewClassifier (const char* filename);
  void print_content();
  void classify();
  ~ScrewClassifier ();

};

#endif
