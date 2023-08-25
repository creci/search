#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>
#include <iostream>

#include "threadsafe_queue.h"

class join_threads {
  std::vector<std::thread> &threads;

public:
  explicit join_threads(std::vector<std::thread> &threads_)
      : threads(threads_) {}
  ~join_threads() {
    for (auto i = 0u; i < threads.size(); i++) {
      if (threads[i].joinable())
        threads[i].join();
    }
  }
};

class function_wrapper {
  struct impl_base {
    virtual void call() = 0;
    virtual ~impl_base() {}
  };

  std::unique_ptr<impl_base> impl;

  template <typename F> struct impl_type : impl_base {
    F f;
    impl_type(F &&f_) : f(std::move(f_)) {}
    void call() { f(); }
  };

public:
  template <typename F>
  function_wrapper(F &&f) : impl(new impl_type<F>(std::move(f))) {}
  function_wrapper() = default;
  function_wrapper(function_wrapper &&other) : impl(std::move(other.impl)) {}
  function_wrapper(const function_wrapper &) = delete;
  function_wrapper(function_wrapper &) = delete;

  void operator()() { impl->call(); }

  function_wrapper &operator=(function_wrapper &&other) {
    impl = std::move(other.impl);
    return *this;
  }

  function_wrapper &operator=(const function_wrapper &) = delete;
};

class thread_pool {
private:
  std::atomic_bool done;
  threadsafe_queue<function_wrapper> work_queue;
  std::vector<std::thread> threads;
  join_threads joiner;


  void worker_thread();

public:

  explicit thread_pool(
      size_t thread_count = std::thread::hardware_concurrency());


  ~thread_pool();

  template <typename FunctionType>
  std::future<typename std::result_of<FunctionType()>::type>
  submit(FunctionType f) {
    using result_type = typename std::result_of<FunctionType()>::type;

    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res(task.get_future());
    work_queue.push(std::move(task));
    return res;
  }
};

#endif //THREAD_POOL_H