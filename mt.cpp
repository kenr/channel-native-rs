#include "mt.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <random>
#include <stdint.h>
#include <string>
#include <thread>
#include <vector>

callback_t internal_callback;
const void *internal_user_data{nullptr};

std::vector<std::thread> threads;
static std::mutex threads_mtx;
static std::atomic<bool> do_run{true};

static std::mt19937 rng(std::random_device{}());
static std::uniform_int_distribution<uint32_t> dist(0, 63);

void register_callback(callback_t callback, const void *user_data) {
  internal_callback = callback;
  internal_user_data = user_data;
}

std::string random_string(std::string::size_type length) {
  static auto &chrs = "0123456789"
                      "abcdefghijklmnopqrstuvwxyz"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  thread_local static std::mt19937 rg{std::random_device{}()};
  thread_local static std::uniform_int_distribution<std::string::size_type>
      pick(0, sizeof(chrs) - 2);

  std::string s;

  s.reserve(length);

  while (length--)
    s += chrs[pick(rg)];

  return s;
}

void start(const uint32_t number_of_threads) {
  std::lock_guard<std::mutex> lock(threads_mtx);

  for (uint32_t i = 0; i < number_of_threads; i++) {
    threads.emplace_back(std::thread([] {
      do {
        if (internal_callback != nullptr) {
          const auto random_data = random_string(40).c_str();
          internal_callback(random_data, internal_user_data);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      } while (do_run);
    }));
  }
}

void stop() {
  std::lock_guard<std::mutex> lock(threads_mtx);
  do_run = false;

  for (auto &t : threads) {
    t.join();
  }
}
