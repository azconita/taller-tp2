#include "configuration.h"

ScrewConfiguration::ScrewConfiguration(const char* cfg_file) {
  std::string buf_str;
  std::ifstream ifs_cfg(cfg_file);
  while (ifs_cfg.peek() != EOF) {
    std::getline(ifs_cfg, buf_str);
    int numtype = std::stoi(buf_str.substr(0, buf_str.find_first_of('=')));
    std::string nametype = buf_str.substr(buf_str.find_first_of('=') + 1,
                          buf_str.find_first_of(',') - buf_str.find_first_of('=') - 1);
    int limit = std::stoi(buf_str.substr(buf_str.find_first_of(',') + 1));
    this->screw_types.emplace(numtype, std::pair<std::string, int> (nametype,limit));
  }
  ifs_cfg.close();

}
ScrewConfiguration::~ScrewConfiguration() {
  this->screw_types.clear();
}

std::string ScrewConfiguration::get_type(int ntype) {
  return std::get<0>(this->screw_types.find(ntype)->second);
}

int ScrewConfiguration::get_limit(int ntype) {
  return std::get<1>(this->screw_types.find(ntype)->second);
}
