/*
Execute unit tests and performance benchmarks.

Copyright 2020. Siwei Wang.
*/
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include "extended.h"
#include "infinite_error.h"
#include "test.h"
using std::accumulate;
using std::back_inserter;
using std::cout;
using std::default_random_engine;
using std::function;
using std::generate;
using std::ios_base;
using std::make_pair;
using std::pair;
using std::transform;
using std::uniform_int_distribution;
using std::vector;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using dur_t = std::chrono::microseconds;

static constexpr auto dur_s = "microseconds";

/**
 * Generate sz numbers in [min_val, max_val].
 * @param sz The size of the random vector.
 * @param min_val The minimal value.
 * @param max_val The maximal value.
 * @returns A random vector.
 */
template <typename T>
vector<T> random_numbers(size_t sz, T min_val, T max_val);

/**
 * Convert integer vector into extended numbers.
 * @param original The original vector of numbers.
 * @param returns A vector of extended numbers from the original.
 * @returns Original vector in extended form.
 */
template <typename T>
vector<Extended<T>> extend(const vector<T>& original);

/**
 * @param numbers The data to operate on.
 * @returns The sum and product of numbers.
 */
template <typename T>
pair<T, T> operate(const vector<T>& numbers);

int main() {
  ios_base::sync_with_stdio(false);

  cout << "--- UNIT TESTS ---\n";
  vector<pair<const char*, const function<void()>>> tests{
      {"basic functionality", test::basic},
      {"comparison", test::comparison},
      {"unary operators", test::unary},
      {"addition and subtraction", test::add_subtract},
      {"multiplication and division", test::multiply_divide},
      {"finite value operations", test::finite_ops},
      {"stream insertion and extraction", test::stream}};
  size_t counter = 0;
  for (const auto& t_name_func : tests) {
    cout << "Test " << t_name_func.first;
    bool success = true;
    try {
      t_name_func.second();
    } catch (const test_error& t_err) {
      success = false;
      cout << " failed : " << t_err.what() << '\n';
    } catch (const infinite_error& i_err) {
      success = false;
      cout << " failed internally : " << i_err.what() << '\n';
    } catch (...) {
      success = false;
      cout << " failed unexpectedly\n";
    }
    if (success) {
      cout << " succeeded\n";
      ++counter;
    }
  }
  cout << counter << " out of " << tests.size() << " tests passed!\n";

  cout << "\n--- PERFORMANCE BENCHMARKS ---\n";
  constexpr size_t sz = 4000000;
  cout << "Time measured in " << dur_s << " on sample size of " << sz << '\n';
  const auto num_sample = random_numbers<int64_t>(sz, -1000, 1000);
  const auto ext_sample = extend(num_sample);

  const auto begin = high_resolution_clock::now();
  const auto num_result = operate(num_sample);
  const auto middle = high_resolution_clock::now();
  const auto ext_result = operate(ext_sample);
  const auto end = high_resolution_clock::now();

  const auto num_time = duration_cast<dur_t>(middle - begin).count();
  const auto ext_time = duration_cast<dur_t>(end - middle).count();
  cout << "Primitive time: " << num_time << '\n';
  cout << "Extended time: " << ext_time << '\n';

  assert(num_result.first == ext_result.first.value(),
         "Benchmark sums do no agree.");
  assert(num_result.second == ext_result.second.value(),
         "Benchmark products do no agree.");
  cout << "Sanity check succeeded\n";
}

template <typename T>
vector<T> random_numbers(size_t sz, T min_val, T max_val) {
  static const auto seed =
      high_resolution_clock::now().time_since_epoch().count();
  static default_random_engine gen(seed);
  static uniform_int_distribution<T> distr(min_val, max_val);
  vector<T> rand_nums(sz);
  generate(rand_nums.begin(), rand_nums.end(), []() { return distr(gen); });
  return rand_nums;
}

template <typename T>
vector<Extended<T>> extend(const vector<T>& original) {
  vector<Extended<T>> ext;
  ext.reserve(original.size());
  transform(original.begin(), original.end(), back_inserter(ext),
            [](T x) { return Extended<T>(x); });
  return ext;
}

template <typename T>
pair<T, T> operate(const vector<T>& numbers) {
  const auto sum = accumulate(numbers.begin(), numbers.end(), T(0));
  const auto prod = accumulate(numbers.begin(), numbers.end(), T(1),
                               [](T x, T y) { return x * y; });
  return make_pair(sum, prod);
}
