#pragma once

#include <condition_variable>  // NOLINT
#include <functional>
#include <future>  // NOLINT
#include <memory>
#include <mutex>  // NOLINT
#include <queue>
#include <thread>  // NOLINT
#include <utility>
#include <vector>
#include "Common.h"

class ThreadPool {
 public:
  explicit ThreadPool(int size = 10);
  ~ThreadPool();

  DISALLOW_COPY_AND_MOVE(ThreadPool);

  template <class F, class... Args>
  auto AddTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

 private:
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_;
  std::mutex tasks_mutex_;
  std::condition_variable cv_;
  std::atomic<bool> stop_{false};
};

// 不能放在cpp文件，原因是C++编译器不支持模版的分离编译
template <class F, class... Args>
auto ThreadPool::AddTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task =
      std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(tasks_mutex_);

    if (stop_) {
      throw std::runtime_error("enqueue on stopped ThreadPool.");
    }

    tasks_.emplace([task]() { (*task)(); });
  }
  cv_.notify_one();
  return res;
}
