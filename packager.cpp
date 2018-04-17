#include "packager.h"
#include <map>
#include <string>
#include <vector>

ScrewClassifier::ScrewClassifier(const char* filename,
                  ScrewsQueue *classified) :

                  classified(classified){
  this->infile = std::ifstream(filename, std::ios::in | std::ios::binary);
  if (this->infile.fail()) {
    std::cerr << filename << ": no se pudo conectar con el dispositivo" << '\n';
  } else {
    std::getline(this->infile, this->name, '\0');
    std::stringstream s;
    s << filename << ": se establece conexion con el dispositivo "
            << this->name << '\n';
    std::cout << s.str();
  }
}

void ScrewClassifier::run() {
  unsigned char buf[4];
  while (this->infile.peek() != EOF) {
    this->infile.read((char *) buf, 4);
    if (buf[0] == 0xFF && buf[1] == 0xFF && buf[2] == 0xFF && buf[3] == 0xFF) {
      std::cerr << this->name << " atascado" << '\n';
      continue;
    }
    ScrewBlock block(buf);
    //!!!!!!!!!!!!!!!!!
    this->classified->push(block);
  }
  this->finished = true;
}

bool ScrewClassifier::is_finished() {
  return this->finished;
}

ScrewClassifier::~ScrewClassifier() {
  this->infile.close();
}



void ScrewPackager::connect_to_classificators(const int total,
                                              const char *file_classif[]) {
  this->total = total;
  for (int i = 0; i < total; i++) {
    this->classificators.push_back(ScrewClassifier(file_classif[i],
              &(this->classified)));
  }
}

void ScrewPackager::start() {
  //TODO: ver si sirve un for_each acá
  for (int i = 0; i < this->total; i++) {
    this->classificators[i].start();
  }
  this->make_packages();
  for (size_t i = 0; i < this->classificators.size(); i++) {
    this->classificators[i].join();
  }
  this->get_remainder();
}

void ScrewPackager::make_packages() {
  std::map<int, ScrewsUnpackaged>::iterator it;
  bool finish = false;
  while (finish == false || !this->classified.empty()) {
    ScrewBlock block = this->classified.pop();
    it = this->screws_unpackaged.find(block.type);
    if (it == this->screws_unpackaged.end()) {
      this->screws_unpackaged.emplace(block.type,ScrewsUnpackaged(block.type,
                  this->config.get_limit(block.type)));
      it = this->screws_unpackaged.find(block.type);
    }
    if (this->config.has_type(block.type)) {
      for (int j = 0; j < block.quantity; j++) {
        int m = it->second.make_package(block.quantity, block.width,
                        this->config.get_limit(block.type),
                        this->config.get_type(block.type));
        if (m > 0)
          std::cout << "Paquete listo: " << this->config.get_limit(block.type)
                << " tornillos de tipo " << this->config.get_type(block.type)
                << " (mediana: " << m << ")\n";
      }
    } else {
      std::cerr << "Tipo de tornillo invalido: " << block.type << '\n';
    }
    if (this->classified.empty()) {
      bool checkthreads = true;
      for (size_t i = 0; i < this->classificators.size(); i++) {
        if (this->classificators[i].is_finished() == false)
          checkthreads = false;
      }
      finish = checkthreads;
    }
  }
}

void ScrewPackager::get_remainder() {
  std::map<int, ScrewsUnpackaged>::iterator itclas;
  std::cout << "# Informe de remanentes" << '\n';
  //TODO:for_each
  std::vector<int> v = this->config.get_types();
  int tot = 0;
  for (std::vector<int>::iterator itv = v.begin(); itv != v.end(); ++itv) {
    if ((itclas = this->screws_unpackaged.find(*itv)) ==
              this->screws_unpackaged.end()) {
      tot = 0;
    } else {
      tot = itclas->second.get_size();
    }
    std::cout << "* " << tot << " tornillos de tipo "
              << this->config.get_type(*itv) << '\n';
  }
}

ScrewPackager::~ScrewPackager() {
}
