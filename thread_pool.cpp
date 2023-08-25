#include "thread_pool.h"
#include <iostream>

void thread_pool::worker_thread() {
  while (!done) {
    function_wrapper task;
    if (work_queue.try_pop(task)) {
      task();
    } else {
      std::this_thread::yield();
    }
  }
}

thread_pool::thread_pool(size_t thread_count) : done(false), joiner(threads) {
  try {
    for (auto i = 0u; i < thread_count; ++i) {
      threads.push_back(std::thread(&thread_pool::worker_thread, this));
    }
  } catch (...) {
    done = true;
    throw;
  }
}

thread_pool::~thread_pool() { done = true; }