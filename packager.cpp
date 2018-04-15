#include "packager.h"


ScrewClassifier::ScrewClassifier(const char* filename,
                  std::queue<ScrewBlock> *classified) :
                  //infile(filename, std::ios::in | std::ios::binary),
                  classified(classified){
  this->infile = std::ifstream(filename, std::ios::in | std::ios::binary);
  if (this->infile.fail()) {
    std::cerr << filename << ": no se pudo conectar el dispositivo" << '\n';
  } else {
    std::getline(this->infile, this->name, '\0');
    std::cout << filename << ": se establece conexion con el dispositivo "
            << this->name << '\n';
  }
}


void ScrewClassifier::print_content() {
  std::string strBuf;
  std::getline(this->infile, strBuf, '\0');
  std::cout << strBuf;
}

void ScrewClassifier::classify() {
  unsigned char buf[4];
  while (this->infile.peek() != EOF) {
    if (*buf == 0xFFFFFFFF) {
      std::cerr << this->name << " atascado" << '\n';
      break;
    }
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
      if (this->config.has_type(block.type)) {
        it->second.make_package(block.quantity, block.width, this->config.get_limit(block.type), this->config.get_type(block.type));
      } else {
        std::cerr << "Tipo de tornillo invalido: " << block.type << '\n';
      }

      //}
      this->classified.pop();
    } else {
      this->screws_unpackaged.emplace(block.type,ScrewsUnpackaged(block.type, this->config.get_limit(block.type)));
    }
    //std::cout << "type: " << this->config.get_type(block.type) << std::endl;
  }
  this->get_remainder();
}

void ScrewPackager::get_remainder() {
  std::map<int, ScrewsUnpackaged>::iterator itclas;
  std::cout << "# Informe de remanentes" << '\n';
  //TODO:for_each
  std::vector<int> v = this->config.get_types();
  int tot = 0;
  for (std::vector<int>::iterator itv = v.begin(); itv != v.end(); ++itv) {
    if ((itclas = this->screws_unpackaged.find(*itv)) == this->screws_unpackaged.end()) {
      tot = 0;
    } else {
      tot = itclas->second.get_size();
    }
    std::cout << "* " << tot << " de tipo " << this->config.get_type(*itv) << '\n';
  }
}

ScrewPackager::~ScrewPackager() {
  //TODO: hace falta sacar todos los elementos de la lista?
  while (!this->classified.empty()) {
    this->classified.pop();
  }
}
