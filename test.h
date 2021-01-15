/*
Test for extended numeric types.

Copyright 2020. Siwei Wang.
*/
#pragma once
#include <exception>

namespace test {
void basic();
void comparison();
void unary();
void add_subtract();
void multiply_divide();
void finite_ops();
void stream();
}  // namespace test

class test_error : public std::exception {
 private:
  const char* msg;

 public:
  explicit test_error(const char* problem) : msg(problem) {}
  const char* what() const noexcept { return msg; }
};

/**
 * Asserts that predicate is true.
 * THROWS: test_error otherwise.
 * @param predicate The object to assert it true.
 */
void assert(bool predicate, const char* msg);
