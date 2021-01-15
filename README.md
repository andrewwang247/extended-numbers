# Extended Numbers

Adds two points at positive and negative infinity to an existing numeric type. Information on the extended real numbers can be found at <https://en.wikipedia.org/wiki/Extended_real_number_line>. The `Extended` class uses the measure-theoretic definition where the product of `0 * (+/- inf) == 0`.

The implementation is header-only, so one only needs to put `#include "extended.h"`. Then `Extended<T>` extends the number system given by `T`, which must be a non-`bool` arithmetic type. Namely, `std::is_arithmetic_v<T> == true`. The extension is lightweight, adding only a single byte to the memory footprint of `T`.

To run unit tests and performance benchmarks, compile with the included `Makefile` and execute `benchmark`. The extended numbers class boasts full test coverage.

## Basic Functionality

An `Extended<T>` type represents all possible values of `T` with the addition of `+inf` and `-inf`. It can be initialized by default (sets to 0), by value (given some `T`), and by specifying the infinity type. The last option uses the `enum class` objects `INF::POS` and `INF::NEG` to represent positive and negative infinity respectively. An existing `Extended<T>` object may also be assigned to via `T`, `INF`, or another `Extended<T>` object.

Query an extended type for finiteness with the `bool finite()` member function. This function is non-throwing. The `T value()` and `INF infinite_type()` member functions expect that the number is finite and infinite, respectively. Finally, one can statically convert between types using `Extended<S> Extended<T>::as_type<S>()`.

## Comparison and Arithmetic

The value `INF::POS` is strictly greater than all other values of `Extended<T>`, while `INF::NEG` is strictly less than all other values. Finite values of `Extended<T>` defer to the ordering of `T`.

The arithmetic rules of extended number systems follows our intuition of how infinity and limits should behave. See <https://en.wikipedia.org/wiki/Extended_real_number_line#Arithmetic_operations> for a list of all the rules. Note that the `Extended` class uses the measure-theoretic definition of the product of 0 and infinite values being 0.

Errors that result from invalid infinite operations will throw an `infinite_error` with a descriptive error message detailing the problem. Often times, this is an indeterminate form from an arithmetic operation.

## Operations

All unary, increment, decrement, arithmetic, comparision, bitwise, and logical operations that one would expect from the type `T` are defined on `Extended<T>`. Note that bitwise operations and modular arithmetic require finiteness to be well-defined.

Stream insertion can be used to print the usual values of `T` with the special `+inf` and `-inf` reserved for infinite values. Stream extraction works only with finite values to read into `Extended<T>`.
