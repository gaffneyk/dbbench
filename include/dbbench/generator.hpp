#ifndef DBBENCH_GENERATOR_HPP
#define DBBENCH_GENERATOR_HPP

#include <random>
#include <utility>

namespace dbbench {

class Generator {
public:
  Generator() : prng_(std::random_device()()) {}

  template <typename T> T uniform(T a, T b) {
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);

    if (std::is_integral_v<T>) {
      return std::uniform_int_distribution<T>(a, b)(prng_);
    }

    if (std::is_floating_point_v<T>) {
      return std::uniform_real_distribution<T>(a, b)(prng_);
    }
  }

  template <typename T> T uniform() {
    return uniform(std::numeric_limits<T>::min(),
                   std::numeric_limits<T>::max());
  }

  std::minstd_rand &prng() { return prng_; }

  std::string string(size_t n, const std::string &chars);

  template <typename T>
  std::vector<T> sample(size_t n, const std::vector<T> &v) {
    std::vector<T> result;
    std::sample(v.begin(), v.end(), std::back_inserter(result), n, prng_);
    return result;
  }

private:
  std::minstd_rand prng_;
};

} // namespace dbbench

#endif // DBBENCH_GENERATOR_HPP
