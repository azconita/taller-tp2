#include "packager.h"


ScrewClassifier::ScrewClassifier(const char* filename,
                                 std::queue<ScrewBlock> *classified) :
                  infile(filename, std::ios::in | std::ios::binary),classified(classified){
  //*infile = std::ifstream(filename, std::ios::in | std::ios::binary);

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
    ScrewBlock block(buf);
    this->classified->push(block);
  }
}

ScrewClassifier::~ScrewClassifier() {
  this->infile.close();
}
