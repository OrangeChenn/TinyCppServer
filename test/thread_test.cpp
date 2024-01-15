#include <iostream>
#include <string>
#include "ThreadPool.h"

void Print(int a, double b, const char *c, const std::string &d) {
  std::cout << a << " " << b << " " << c << " " << d << std::endl;
}

void Test() { std::cout << "hello ThreadPool" << std::endl; }

int main(int argc, char *argv[]) {
  ThreadPool *pool = new ThreadPool(4);
  std::function<void()> func = std::bind(Print, 1, 1.2, "hello", std::string("ThreadPool"));
  pool->AddTask(func);
  func = Test;
  pool->AddTask(func);
  delete pool;

  return 0;
}
