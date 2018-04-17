#include "configuration.h"
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <fstream>

ScrewConfiguration::ScrewConfiguration(const char* cfg_file) {
  std::string buf_str;
  std::ifstream ifs_cfg(cfg_file);
  while (ifs_cfg.peek() != EOF) {
    std::getline(ifs_cfg, buf_str);
    int type = std::stoi(buf_str.substr(0, buf_str.find_first_of('=')));
    std::string name = buf_str.substr(buf_str.find_first_of('=') + 1,
                buf_str.find_first_of(',') - buf_str.find_first_of('=') - 1);
    int limit = std::stoi(buf_str.substr(buf_str.find_first_of(',') + 1));
    this->screw_types.emplace(type, std::pair<std::string, int> (name,limit));
  }
  ifs_cfg.close();
}

ScrewConfiguration::~ScrewConfiguration() {
  this->screw_types.clear();
}

std::vector<int> ScrewConfiguration::get_types() {
  std::vector<int> v;
  for (std::map<int,std::pair<std::string,int>>::iterator it =
        this->screw_types.begin(); it != this->screw_types.end(); ++it)
    v.push_back(it->first);
  return v;
}

std::string ScrewConfiguration::get_type(int ntype) {
  return std::get<0>(this->screw_types.find(ntype)->second);
}

bool ScrewConfiguration::has_type(int ntype) {
  std::map<int,std::pair<std::string,int>>::iterator it =
            this->screw_types.find(ntype);
  if (it != this->screw_types.end()) {
    return true;
  } else {
    return false;
  }
}

int ScrewConfiguration::get_limit(int ntype) {
  return std::get<1>(this->screw_types.find(ntype)->second);
}
