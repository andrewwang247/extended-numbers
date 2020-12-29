/*
Exception for errors with infinity.

Copyright 2020. Siwei Wang.
*/
#include "infinite_error.h"

infinite_error::infinite_error(const char* prob) : msg(prob) {}

const char* infinite_error::what() const noexcept { return msg; }

void inf_assert(bool pred, const char* msg) {
  if (!pred) throw infinite_error(msg);
}
