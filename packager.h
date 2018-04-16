#ifndef SCREW_PACKAGER_H
#define SCREW_PACKAGER_H

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <algorithm>
#include "configuration.h"
#include "screwblock.h"
#include "printers.h"

class ScrewsQueue {
private:
  std::queue<ScrewBlock> q;
  std::mutex m;
  std::condition_variable cv;
public:
  ScrewsQueue () {}
  virtual ~ScrewsQueue () {}
  ScrewBlock pop() {
    std::unique_lock<std::mutex> l(m);
    while (q.empty())
      cv.wait(l);
    ScrewBlock b = this->q.front();
    this->q.pop();
    return b;
  }
  ScrewBlock front() {
    std::unique_lock<std::mutex> l(m);
    ScrewBlock b = this->q.front();
    return b;
  }
  void push(ScrewBlock const block) {
    std::lock_guard<std::mutex> l(m);
    bool wake = q.empty();
    this->q.push(block);
    if (wake)
      cv.notify_one();
  }
  bool empty() {
    std::unique_lock<std::mutex> l(m);
    return this->q.empty();
  }
};

class Thread {
  private:
  std::thread thread;

  public:
  Thread () {}
  virtual ~Thread () {}
  void start() {
    this->thread = std::thread(&Thread::run, this);
  }
  void join() {
    this->thread.join();
  }
  virtual void run() = 0;
  //deberia hacer la copia por movimiento?
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  Thread(Thread&& other) {
    this->thread = std::move(other.thread);
  }
  Thread& operator=(Thread&& other) {
    this->thread = std::move(other.thread);
    return *this;
  }
};

class ScrewClassifier : public Thread {
 private:
  ErrPrinter *perr;
  OutPrinter *pout;
  std::ifstream infile;
  std::string name;
  ScrewsQueue *classified;
  bool finished = false;
 public:
  ScrewClassifier (const char* filename, ScrewsQueue *classified, ErrPrinter *pe, OutPrinter *po);
  ScrewClassifier(ScrewClassifier &&other) : infile(std::move(other.infile)), name(other.name), classified(other.classified) {}
  void run();
  ~ScrewClassifier();
  bool is_finished();
};

class ScrewsUnpackaged {
 private:
  int total = 0;
  std::vector<int> widths;
  int limit = 0;
  int type;
  ErrPrinter *perr;
  OutPrinter *pout;
 public:
  ScrewsUnpackaged (int type, int limit, ErrPrinter *pe, OutPrinter *po) : type(type), limit(limit), perr(pe), pout(po) {}
  virtual ~ScrewsUnpackaged () {
    this->widths.clear();
  }
  int get_type() {
    return this->type;
  }
  int get_size() {
    return this->widths.size();
  }
  int make_package(int quantity, int width, int limit, std::string btype) {
    int left = quantity;
    int packages = 0;
    for (size_t i = 0; i < quantity; i++) {
      this->widths.push_back(width);
      if (this->widths.size() == limit) {
        int m = this->get_median(limit);
        //std::mutex mscreen.lock();
        std::stringstream s;
        s << "Paquete listo: " << limit << " tornillos de tipo "
                  << btype << " (mediana: " << m << ")\n";
        this->pout->print(&s);
        //mscreen.unlock();
        ++packages;
        this->widths.clear();
      }
    }
    return packages;
  }
  int get_median(int limit) {
    std::sort(this->widths.begin(),this->widths.end());
    if (limit % 2 == 0)
      return (this->widths[limit / 2 - 1] + this->widths[limit / 2]) / 2;
    else
      return this->widths[limit / 2];
  }
};

class ScrewPackager {
 private:
  ErrPrinter *perr;
  OutPrinter *pout;
  ScrewConfiguration config;
  ScrewsQueue classified;
  std::vector<ScrewClassifier> classificators;
  int total = 0;
  std::map<int, ScrewsUnpackaged> screws_unpackaged;
 public:
  ScrewPackager (const char * cfg_file) ;
  virtual ~ScrewPackager ();
  void connect_to_classificators(const int total, const char *file_classif[]);
  void start();
 private:
  void make_packages();
  void get_remainder();
};

#endif
