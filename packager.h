#ifndef SCREW_PACKAGER_H
#define SCREW_PACKAGER_H

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <queue>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <algorithm>
#include "configuration.h"
#include "screwblock.h"

class ScrewsQueue {
private:
  std::queue<ScrewBlock> q;
  std::mutex m;
  std::condition_variable cv;

public:
  ScrewsQueue() {}
  virtual ~ScrewsQueue() {}
  ScrewBlock pop() {
    std::unique_lock<std::mutex> l(this->m);
    while (this->q.empty())
      this->cv.wait(l);
    ScrewBlock b = this->q.front();
    this->q.pop();
    return b;
  }
  void push(ScrewBlock const block) {
    std::unique_lock<std::mutex> l(this->m);
    bool empty = this->q.empty();
    this->q.push(block);
    l.unlock();
    if (empty)
      this->cv.notify_all();
  }
  bool empty() {
    std::unique_lock<std::mutex> l(this->m);
    return this->q.empty();
  }
};

class Thread {
  private:
  std::thread thread;

  public:
  Thread() {}
  virtual ~Thread() {}
  void start() {
    this->thread = std::thread(&Thread::run, this);
  }
  void join() {
    this->thread.join();
  }
  virtual void run() = 0;
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
  std::ifstream infile;
  std::string name;
  ScrewsQueue *classified;
  bool finished = false;

 public:
  ScrewClassifier(const char* filename, ScrewsQueue *classified);
  ScrewClassifier(ScrewClassifier &&other) : infile(std::move(other.infile)),
    name(other.name), classified(other.classified), finished(other.finished) {}
  virtual void run() override;
  ~ScrewClassifier();
  bool is_finished();
};

class ScrewsUnpackaged {
 private:
  int total = 0;
  std::vector<int> widths;
  const int type;
  const unsigned int limit;

 public:
  ScrewsUnpackaged(int type, int limit) : type(type), limit(limit) {}
  virtual ~ScrewsUnpackaged() {
    this->widths.clear();
  }
  int get_type() {
    return this->type;
  }
  int get_size() {
    return this->widths.size();
  }
  int make_package(int quantity, int width, unsigned int limit,
                    std::string btype) {
    this->widths.push_back(width);
    if (this->widths.size() == limit) {
      int m = this->get_median(limit);
      this->widths.clear();
      return m;
    }
    return -1;
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
  ScrewConfiguration config;
  ScrewsQueue classified;
  std::vector<ScrewClassifier> classificators;
  int total = 0;
  std::map<int, ScrewsUnpackaged> screws_unpackaged;

 public:
  explicit ScrewPackager(const char * cfg_file) : config(cfg_file) {}
  virtual ~ScrewPackager();
  void connect_to_classificators(const int total, const char *file_classif[]);
  void start();

 private:
  void make_packages();
  void get_remainder();
};

#endif
