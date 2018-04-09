#include "classifier.hpp"


ScrewClassifier::ScrewClassifier(const char* filename) :
                  infile(filename, std::ios::in | std::ios::binary) {
}

void ScrewClassifier::print_content() {
  std::string strBuf;
  std::getline(this->infile, strBuf, '\0');
  std::cout << strBuf;
}

void ScrewClassifier::classify() {
  unsigned char buf[4];
  std::getline(this->infile, this->name, '\0');
  while (this->infile.peek() != EOF) {
    //TODO: ver que no haya error en read
    this->infile.read((char *) buf, 4);
/*    std::cout << std::hex << (int) buf [0];
    std::cout << std::hex << (int) buf [1];
    std::cout << std::hex << (int) buf [2];
    std::cout << std::hex << (int) buf [3] << std::endl;*/
    ScrewBlock block(buf);
    this->classified.push(block);
  }
}

ScrewClassifier::~ScrewClassifier() {
  //TODO: hace falta sacar todos los elementos de la lista?
  while (!this->classified.empty()) {
    this->classified.pop();
  }
  this->infile.close();
}
