#include "packager.h"


ScrewClassifier::ScrewClassifier(const char* filename,
                  std::queue<ScrewBlock> *classified) :
                  infile(filename, std::ios::in | std::ios::binary),
                  classified(classified){
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


void ScrewPackager::connect_to_classificators(const int total,
                                              const char *file_classif[]) {
  this->total = total;
  for (size_t i = 0; i < total; i++) {
    classificators.push_back(ScrewClassifier(file_classif[i], &(this->classified)));
  }
}

void ScrewPackager::start() {
  //TODO: ver si sirve un for_each acá
  for (size_t i = 0; i < this->total; i++) {
    this->classificators[i].classify();
  }
  this->make_packages();
}

void ScrewPackager::make_packages() {

  std::map<int, ScrewsUnpackaged>::iterator it;
  while (!this->classified.empty()) {
    //std::cout << "making packages" << '\n';
    ScrewBlock block = this->classified.front();
    it = this->screws_unpackaged.find(block.type);
    if (it != this->screws_unpackaged.end()) {
      //if (it->second.get_size() + block.quantity >= this->config.get_limit(block.type)) {
        it->second.make_package(block.quantity, block.width, this->config.get_limit(block.type), this->config.get_type(block.type));
      //}
      this->classified.pop();
    } else {
      this->screws_unpackaged.emplace(block.type,ScrewsUnpackaged(block.type, this->config.get_limit(block.type)));
    }
    //std::cout << "type: " << this->config.get_type(block.type) << std::endl;
  }
  std::cout << "Remanentes" << '\n';
  //TODO:for_each
  it = this->screws_unpackaged.begin();
  for (; it != this->screws_unpackaged.end(); ++it) {
    std::cout << this->config.get_type(it->second.get_type()) << " " << it->second.get_size() << '\n';
  }
}



ScrewPackager::~ScrewPackager() {
  //TODO: hace falta sacar todos los elementos de la lista?
  while (!this->classified.empty()) {
    this->classified.pop();
  }
}
