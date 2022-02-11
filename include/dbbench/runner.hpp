#ifndef DBBENCH_RUNNER_HPP
#define DBBENCH_RUNNER_HPP

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

namespace dbbench {

struct State {
  std::thread thread;
  std::atomic_size_t count = 0;
};

template <typename Worker>
double run(std::vector<Worker> &workers, size_t s_warmup, size_t s_measure) {
  std::vector<State> states(workers.size());

  std::atomic_bool go = true;

  for (size_t i = 0; i < workers.size(); ++i) {
    states[i].thread = std::thread([&, i] {
      while (go) {
        bool success = workers[i]();
        if (success) {
          ++states[i].count;
        }
      }
    });
  }

  std::this_thread::sleep_for(std::chrono::seconds(s_warmup));

  size_t warmup_count = 0;
  for (State &state : states) {
    warmup_count += state.count;
  }

  std::this_thread::sleep_for(std::chrono::seconds(s_measure));

  size_t total_count = 0;
  for (State &state : states) {
    total_count += state.count;
  }

  go = false;
  for (State &state : states) {
    state.thread.join();
  }

  size_t measure_count = total_count - warmup_count;
  return (double)measure_count / (double)s_measure;
}

} // namespace dbbench

#endif // DBBENCH_RUNNER_HPP
