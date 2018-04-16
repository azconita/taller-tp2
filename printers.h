#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <condition_variable>

class ErrPrinter {
private:
  std::mutex m;
  std::condition_variable cv;

public:
  ErrPrinter () {}
  virtual ~ErrPrinter () {}
  void print(std::stringstream &s) {
    std::lock_guard<std::mutex> l(m);
    std::cerr << s.str;
  }
};

class OutPrinter {
private:
  std::mutex m;
  std::condition_variable cv;

public:
  OutPrinter () {}
  virtual ~OutPrinter () {}
  void print(std::stringstream &s) {
    std::lock_guard<std::mutex> l(m);
    std::cout << s.str();
  }
};
