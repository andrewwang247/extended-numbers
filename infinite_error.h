/*
Exception for errors with infinity.

Copyright 2020. Siwei Wang.
*/
#pragma once
#include <exception>

class infinite_error : public std::exception {
 private:
  const char* msg;

 public:
  /**
   * Construct error with given problem.
   * @param prob Error message.
   */
  explicit infinite_error(const char* prob);

  /**
   * @returns The error message.
   */
  const char* what() const noexcept;
};

/**
 * Checks pred, otherwise throws infinite_error with msg.
 * @param pred Asserts is true.
 * @param msg The error message to throw otherwise.
 * THROWS: infinite_error if pred is false.
 */
void inf_assert(bool pred, const char* msg);
