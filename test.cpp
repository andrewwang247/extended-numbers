/*
Test for extended numeric types.

Copyright 2020. Siwei Wang.
*/
#include "test.h"
#include <sstream>
#include <string>
#include <vector>
#include "extended.h"
using std::string;
using std::stringstream;
using std::vector;

void assert(bool predicate, const char* msg) {
  if (!predicate) throw test_error(msg);
}

/**
 * Checks that numbers are close.
 * @param num_1 The first value.
 * @param num_2 The second value.
 * @param epsilon The margin of error.
 * @returns If |num_1 - num_2| < epsilon.
 */
template <typename S, typename T>
bool close(S num_1, T num_2, double epsilon = 0.00001) {
  const auto diff = num_2 - num_1;
  return (-epsilon < diff) && (diff < epsilon);
}

/**
 * Feeds ext into a stringstream to test stream insertion.
 * @param ext An extended number.
 * @returns The extracted string of ext.
 */
template <typename T>
string extract(const Extended<T>& ext) {
  stringstream ss;
  ss << ext;
  return ss.str();
}

void test::basic() {
  const Extended<unsigned char> empty;
  assert(empty.finite(), "Default init is finite.");
  assert(empty.value() == static_cast<unsigned char>(0),
         "Default init has value 0.");
  bool thrown = false;
  try {
    empty.infinite_type();
  } catch (const infinite_error&) {
    thrown = true;
  }
  assert(thrown, "Default init is not infinite.");

  Extended<unsigned> fin(static_cast<unsigned>(3));
  assert(fin.finite(), "Value init is finite.");
  assert(fin.value() == 3, "Value init has value 3.");
  thrown = false;
  try {
    fin.infinite_type();
  } catch (const infinite_error&) {
    thrown = true;
  }
  assert(thrown, "Value init is not infinite.");

  const auto ln = fin.as_type<int64_t>();
  assert(ln.finite(), "Converted value is finite.");
  assert(ln.value() == 3, "Converted has value 3.");
  thrown = false;
  try {
    ln.infinite_type();
  } catch (const infinite_error&) {
    thrown = true;
  }
  assert(thrown, "Converted value is not infinite.");

  Extended<int> neg_inf(INF::NEG);
  assert(!neg_inf.finite(), "Negative inf init is not finite.");
  thrown = false;
  try {
    neg_inf.value();
  } catch (const infinite_error&) {
    thrown = true;
  }
  assert(thrown, "Negative inf init has no finite value.");
  assert(neg_inf.infinite_type() == INF::NEG,
         "Negative inf init has negative inf type.");

  const auto convert = neg_inf.as_type<char>();
  assert(!convert.finite(), "Converted neg inf is not finite.");
  thrown = false;
  try {
    convert.value();
  } catch (const infinite_error&) {
    thrown = true;
  }
  assert(thrown, "Negative inf init has no finite value.");
  assert(convert.infinite_type() == INF::NEG,
         "Converted neg inf has negative inf type.");

  Extended<unsigned char> pos_inf(INF::POS);
  assert(!pos_inf.finite(), "Positive inf init is not finite.");
  thrown = false;
  try {
    pos_inf.value();
  } catch (const infinite_error&) {
    thrown = true;
  }
  assert(thrown, "Positive inf init has no finite value.");
  assert(pos_inf.infinite_type() == INF::POS,
         "Positive inf init has positive inf type.");
}

void test::comparison() {
  const vector<Extended<int>> nums{Extended<int>(INF::NEG), Extended<int>(-42),
                                   Extended<int>(), Extended<int>(42),
                                   Extended<int>(INF::POS)};
  for (size_t i = 0; i < nums.size(); ++i) {
    for (size_t j = 0; j < nums.size(); ++j) {
      const auto& a = nums[i];
      const auto& b = nums[j];
      if (i == j) {
        assert(a == b, "x == x");
        assert(!(a != b), "!(x != x)");
        assert(!(a < b), "!(x < x)");
        assert(a <= b, "x <= x");
        assert(!(a > b), "!(x > x)");
        assert(a >= b, "x >= x");
      } else if (i < j) {
        assert(!(a == b), "!(a == b)");
        assert(a != b, "a != b");
        assert(a < b, "a < b");
        assert(a <= b, "a <= b");
        assert(!(a > b), "!(a > b)");
        assert(!(a >= b), "!(a >= b)");
      } else {
        assert(!(a == b), "!(a == b)");
        assert(a != b, "a != b");
        assert(!(a < b), "a < b");
        assert(!(a <= b), "a <= b");
        assert(a > b, "!(a > b)");
        assert(a >= b, "!(a >= b)");
      }
    }
  }
}

void test::unary() {
  const vector<Extended<char>> nums{
      Extended<char>(INF::NEG), Extended<char>(-42), Extended<char>(),
      Extended<char>(42), Extended<char>(INF::POS)};
  for (size_t i = 0; i < nums.size(); ++i) {
    const auto& a = nums[i];
    const auto& b = nums[nums.size() - i - 1];
    assert(a == +a, "Unary + operator does nothing.");
    assert(a == -b, "Unary - operator inverts sign.");
    assert(-a == b, "Unary - operator inverts sign.");
    if (i == nums.size() / 2)
      assert(!a, "Boolean conversion of 0 is false.");
    else
      assert(a, "Boolean conversion of non-zero is true.");
  }
  const vector<Extended<char>> nums_pp{
      Extended<char>(INF::NEG), Extended<char>(-41), Extended<char>(1),
      Extended<char>(43), Extended<char>(INF::POS)};
  for (size_t i = 0; i < nums.size(); ++i) {
    auto num = nums[i];
    const auto orig = num++;
    assert(orig == nums[i], "Postfix increment should return itself.");
    assert(num == nums_pp[i], "Postfix increment should add 1.");
    auto num_2 = nums[i];
    const auto incr = ++num_2;
    assert(incr == nums_pp[i], "Prefix increment should return +1.");
  }
  const vector<Extended<char>> nums_mm{
      Extended<char>(INF::NEG), Extended<char>(-43), Extended<char>(-1),
      Extended<char>(41), Extended<char>(INF::POS)};
  for (size_t i = 0; i < nums.size(); ++i) {
    auto num = nums[i];
    const auto orig = num--;
    assert(orig == nums[i], "Postfix increment should return itself.");
    assert(num == nums_mm[i], "Postfix increment should add 1.");
    auto num_2 = nums[i];
    const auto decr = --num_2;
    assert(decr == nums_mm[i], "Prefix increment should return +1.");
  }
}

void test::add_subtract() {
  const Extended<int8_t> pos(42);
  const Extended<int8_t> neg(-42);
  const Extended<int8_t> pos_inf(INF::POS);
  const Extended<int8_t> neg_inf(INF::NEG);
  const Extended<int8_t> zero(0);

  assert(pos + neg == zero && zero == neg + pos, "Additive inverse yields 0.");
  assert(pos - neg == -(neg - pos), "Flipping subtraction yields inverse.");
  assert(pos + zero == pos && neg + zero == neg,
         "Adding zero yields original value.");

  for (const auto ptr : {&pos, &neg, &zero, &pos_inf}) {
    assert(pos_inf + *ptr == pos_inf,
           "Positive infinity invariant under addition.");
    assert(*ptr - neg_inf == pos_inf,
           "Subtraction by negative infinity yields positive infinity.");
    assert(neg_inf - *ptr == neg_inf,
           "Negative infinity invariant under subtraction.");
  }

  for (const auto ptr : {&pos, &neg, &zero, &neg_inf}) {
    assert(pos_inf - *ptr == pos_inf,
           "Positive infinity invariant under subtraction.");
    assert(*ptr - pos_inf == neg_inf,
           "Subtraction by positive infinity yields negative infinity.");
    assert(*ptr + neg_inf == neg_inf,
           "Negative infinity invariant under addition.");
  }

  bool pp = false, pn = false, np = false, nn = false;
  try {
    pos_inf - pos_inf;
  } catch (const infinite_error&) {
    pp = true;
  }
  try {
    pos_inf + neg_inf;
  } catch (const infinite_error&) {
    pn = true;
  }
  try {
    neg_inf + pos_inf;
  } catch (const infinite_error&) {
    np = true;
  }
  try {
    neg_inf - neg_inf;
  } catch (const infinite_error&) {
    nn = true;
  }
  for (auto err : {pp, pn, np, nn})
    assert(err, "Indeterminate form involving add/subtract infinity.");
}

void test::multiply_divide() {
  const Extended<int16_t> pos(42);
  const Extended<int16_t> neg(-42);
  const Extended<int16_t> pos_inf(INF::POS);
  const Extended<int16_t> neg_inf(INF::NEG);
  const Extended<int16_t> zero(0);
  const Extended<int16_t> one(1);

  for (const auto ptr : {&pos, &neg, &pos_inf, &neg_inf, &zero, &one}) {
    const auto val = *ptr;
    assert(val * one == val && val == one * val,
           "Multiplying one yields original value.");
    assert(val * zero == zero && zero == zero * val,
           "Multiplying zero yields zero.");
    assert(val / one == val, "Division by one yields original value.");
  }
  const Extended<int32_t> prod(42 * 42);
  assert(-pos * neg == prod && pos * neg == -prod,
         "Finite multiplication error.");
  assert(pos / neg == -one && one == neg / -pos, "Finite division error.");

  for (const auto inf_ptr : {&pos_inf, &neg_inf}) {
    const auto inf = *inf_ptr;
    assert(inf / pos == inf,
           "Infinity invariant under finite positive division.");
    assert(inf / neg == -inf,
           "Infinity flips sign under finite negative division.");
    for (const auto ptr : {&pos, &pos_inf}) {
      assert(inf * (*ptr) == inf,
             "Infinity invariant under positive multiplication.");
    }
    for (const auto ptr : {&neg, &neg_inf}) {
      assert(*ptr * inf == -inf,
             "Infinite flips sign under negative multiplication.");
    }
  }
  for (const auto ptr : {&pos, &neg, &zero, &one}) {
    assert(*ptr / pos_inf == zero,
           "Division by positive infinity yields zero.");
    assert(*ptr / neg_inf == zero,
           "Division by negative infinity yields zero.");
  }
}

void test::finite_ops() {
  vector<Extended<uint32_t>> nums;
  constexpr uint32_t sz = 200;
  nums.reserve(sz);
  for (uint32_t it = 0; it < sz; ++it) {
    nums.emplace_back(it);
  }

  for (size_t i = 0; i < sz; ++i) {
    const auto num_1 = nums[i];
    assert((~num_1).value() == ~(num_1.value()), "Bitwise negation commutes.");
    for (size_t j = 0; j < sz; ++j) {
      const auto num_2 = nums[j];
      if (num_2 != 0)
        assert((num_1 % num_2).value() == num_1.value() % num_2.value(),
               "Modular arithmetic commutes.");
      assert((num_1 & num_2).value() == (num_1.value() & num_2.value()),
             "Bitwise and commutes.");
      assert((num_1 | num_2).value() == (num_1.value() | num_2.value()),
             "Bitwise or commutes.");
      assert((num_1 ^ num_2).value() == (num_1.value() ^ num_2.value()),
             "Bitwise xor commutes.");
      assert((num_1 << num_2).value() == (num_1.value() << num_2.value()),
             "Bitwise leftshift commutes.");
      assert((num_1 >> num_2).value() == (num_1.value() >> num_2.value()),
             "Bitwise rightshift commutes.");
    }
  }
}

void test::stream() {
  stringstream ss;
  Extended<uint16_t> ue(256);
  ss << ue;
  assert(ss.str() == "256", "Unsigned stream insertion fail.");
  Extended<double> de(INF::POS);
  ss >> de;
  assert(close(256, de.value()), "Floating point stream extraction fail.");

  de = INF::NEG;
  assert(extract(de) == "-inf", "Negative infinity extraction fail.");
  ue = INF::POS;
  assert(extract(ue) == "+inf", "Positive infinity extraction fail.");

  stringstream ins;
  Extended<int16_t> neg(-480);
  ins << neg;
  assert(ins.str() == "-480", "Signed stream extraction fail.");
  Extended<int64_t> big(INF::NEG);
  assert(extract(big) == "-inf", "Negative infinity extraction fail.");
  ins >> big;
  assert(big.value() == -480, "Signed stream insertion fail.");
}
