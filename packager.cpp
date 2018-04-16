#include "packager.h"


ScrewClassifier::ScrewClassifier(const char* filename,
                  ScrewsQueue *classified, ErrPrinter *pe, OutPrinter *po) :
                  perr(pe), pout(po),
                  classified(classified){
  this->infile = std::ifstream(filename, std::ios::in | std::ios::binary);
  if (this->infile.fail()) {
    //std::mutex mscreener.lock();
    std::cerr << filename << ": no se pudo conectar el dispositivo" << '\n';
    //mscreener.unlock();
  } else {
    std::getline(this->infile, this->name, '\0');
    //std::mutex mscreen.lock();
    std::stringstream s;
    s << filename << ": se establece conexion con el dispositivo "
            << this->name << '\n';
    std::cout << s.str();
    //mscreen.unlock();
  }
}

void ScrewClassifier::run() {
  unsigned char buf[4];
  while (this->infile.peek() != EOF) {
    if (buf[0] == 0xFF && buf[1] == 0xFF && buf[2] == 0xFF && buf[3] == 0xFF) {
      std::cerr << this->name << " atascado" << '\n';
    }
    this->infile.read((char *) buf, 4);
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

ScrewPackager::ScrewPackager (const char * cfg_file) : config(cfg_file) {
  this->perr = new ErrPrinter();
  this->pout = new OutPrinter();
}


void ScrewPackager::connect_to_classificators(const int total,
                                              const char *file_classif[]) {
  this->total = total;
  for (size_t i = 0; i < total; i++) {
    this->classificators.push_back(ScrewClassifier(file_classif[i], &(this->classified), &(this->perr), &(this->pout)));
  }
}

void ScrewPackager::start() {
  //TODO: ver si sirve un for_each acá
  for (size_t i = 0; i < this->total; i++) {
    this->classificators[i].start();
  }
  this->make_packages();
  this->get_remainder();
}

void ScrewPackager::make_packages() {
  std::map<int, ScrewsUnpackaged>::iterator it;
  bool finish = false;
  while (finish == false) {
    ScrewBlock block = this->classified.pop();
    this->classified.push(block);
    it = this->screws_unpackaged.find(block.type);
    if (it != this->screws_unpackaged.end()) {
      if (this->config.has_type(block.type)) {
        int p = it->second.make_package(block.quantity, block.width, this->config.get_limit(block.type), this->config.get_type(block.type));
        for (size_t j = 0; j < p; j++) {
          std::stringstream s;
          s << "Paquete listo: " << this->config.get_limit(block.type) << " tornillos de tipo \n";
                  //<< btype << " (mediana: " << m << ")\n";
          std::cout << s.str();
        }
      } else {
        std::cerr << "Tipo de tornillo invalido: " << block.type << '\n';
      }
      this->classified.pop();
    } else {
      this->screws_unpackaged.emplace(block.type,ScrewsUnpackaged(block.type, this->config.get_limit(block.type)), &(this->perr), &(this->pout));
    }
    bool checkthreads = true;
    for (size_t i = 0; i < this->classificators.size(); i++) {
      if (this->classificators[i].is_finished() == false)
        checkthreads = false;
    }
    finish = checkthreads;
  }
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
    std::cout << "* " << tot << " tornillos de tipo " << this->config.get_type(*itv) << '\n';
  }
}

ScrewPackager::~ScrewPackager() {
  //TODO: hace falta sacar todos los elementos de la lista?
  /*while (!this->classified.empty()) {
    this->classified.pop();
  }*/
  for (size_t i = 0; i < this->classificators.size(); i++) {
    this->classificators[i].join();
  }
  delete this->perr;
  delete this->pout;
}
