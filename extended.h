/*
Models the extended real numbers for any non-bool arithmetic type T.
See https://en.wikipedia.org/wiki/Extended_real_number_line
Extended<T> abides by measure-theoretic arithmetic rules.

Copyright 2020. Siwei Wang.
*/
#pragma once
#include <iostream>
#include <type_traits>
#include "infinite_error.h"

/**
 * Used to designate positive and negative infinity.
 */
enum class INF : bool { POS = true, NEG = false };

/**
 * Extended number system for type T.
 */
template <typename T>
class Extended {
  using value_type = T;
  static_assert(std::is_arithmetic_v<T>, "Extended type must be numeric.");
  static_assert(!std::is_same_v<T, bool>, "Extended type cannot be bool.");

 private:
  // Internal finite value.
  T m_value;

  // Flag for signalling infinite values.
  char m_flag;

  static constexpr char FINITE_FLAG = 0;
  static constexpr char POS_INF_FLAG = 1;
  static constexpr char NEG_INF_FLAG = -1;

 public:
  // CONSTRUCTION

  /**
   * Zero-initialized finite value.
   */
  Extended() : m_value(static_cast<T>(0)), m_flag(FINITE_FLAG) {}

  /**
   * Parameter-initialized finite value.
   * @param number The initial value.
   */
  explicit Extended(T number) : m_value(number), m_flag(FINITE_FLAG) {}

  /**
   * Infinity-intialized value.
   * @param inf_type either INF::POS or INF::NEG.
   */
  explicit Extended(INF inf_type)
      : m_value(static_cast<T>(0)),
        m_flag(inf_type == INF::POS ? POS_INF_FLAG : NEG_INF_FLAG) {}

  // ASSIGNMENT

  /**
   * Assign from finite value.
   * @param number The value to give to this.
   * @returns Reference to this.
   */
  Extended& operator=(T number) {
    m_value = number;
    m_flag = FINITE_FLAG;
    return *this;
  }

  /**
   * Assign value to positive or negative infinite.
   * @param inf_type either INF::POS or INF::NEG.
   * @returns Reference to this.
   */
  Extended& operator=(INF inf_type) {
    m_flag = (inf_type == INF::POS ? POS_INF_FLAG : NEG_INF_FLAG);
    return *this;
  }

  // BASIC FUNCTIONALITY

  /**
   * @returns Whether this is finite.
   */
  bool finite() const noexcept { return m_flag == FINITE_FLAG; }

  /**
   * REQUIRES: This is finite.
   * @returns The finite value.
   */
  T value() const {
    inf_assert(finite(), "Finite error: This is infinite.");
    return m_value;
  }

  /**
   * REQUIRES: This is infinite.
   * @returns The type of infinite.
   */
  INF infinite_type() const {
    inf_assert(!finite(), "Finite error: This is finite.");
    return m_flag == POS_INF_FLAG ? INF::POS : INF::NEG;
  }

  /**
   * Convert from one type to another.
   * @returns A casted extended number.
   */
  template <typename S>
  Extended<S> as_type() const {
    static_assert(std::is_arithmetic_v<S>, "Extended type must be numeric.");
    static_assert(!std::is_same_v<S, bool>, "Extended type cannot be bool.");
    return finite() ? Extended<S>(static_cast<S>(m_value))
                    : Extended<S>(m_flag == POS_INF_FLAG ? INF::POS : INF::NEG);
  }

  // COMPARISON

  friend bool operator==(const Extended& num_1,
                         const Extended& num_2) noexcept {
    if (num_1.finite() && num_2.finite()) return num_1.m_value == num_2.m_value;
    return num_1.m_flag == num_2.m_flag;
  }

  friend bool operator!=(const Extended& num_1,
                         const Extended& num_2) noexcept {
    return !(num_1 == num_2);
  }

  friend bool operator<(const Extended& num_1, const Extended& num_2) noexcept {
    if (num_1.finite() && num_2.finite()) return num_1.m_value < num_2.m_value;
    if (num_1.finite()) {
      return num_2.m_flag == POS_INF_FLAG;
    }
    if (num_2.finite()) {
      return num_1.m_flag == NEG_INF_FLAG;
    }
    return num_1.m_flag == NEG_INF_FLAG && num_2.m_flag == POS_INF_FLAG;
  }

  friend bool operator>(const Extended& num_1, const Extended& num_2) noexcept {
    return num_2 < num_1;
  }

  friend bool operator<=(const Extended& num_1,
                         const Extended& num_2) noexcept {
    return !(num_2 < num_1);
  }

  friend bool operator>=(const Extended& num_1,
                         const Extended& num_2) noexcept {
    return !(num_1 < num_2);
  }

  // UNARY +/- AND BOOL

  Extended operator+() const noexcept { return Extended(*this); }

  Extended operator-() const noexcept {
    static_assert(std::is_signed_v<T>,
                  "Unary negation only works on signed type.");
    Extended neg;
    if (finite())
      neg.m_value = static_cast<T>(-m_value);
    else
      neg.m_flag = static_cast<char>(-m_flag);
    return neg;
  }

  operator bool() const noexcept {
    if (!finite()) return true;
    return m_value != static_cast<T>(0);
  }

  // INCREMENT / DECREMENT

  Extended& operator++() noexcept {
    ++m_value;
    return *this;
  }

  Extended operator++(int) noexcept {
    const auto tmp(*this);
    ++m_value;
    return tmp;
  }

  Extended& operator--() noexcept {
    --m_value;
    return *this;
  }

  Extended operator--(int) noexcept {
    const auto tmp(*this);
    --m_value;
    return tmp;
  }

  // ARITHMETIC

  Extended& operator+=(const Extended& other) {
    switch (m_flag) {
      case FINITE_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
            m_value = static_cast<T>(m_value + other.m_value);
            break;
          case POS_INF_FLAG:
          case NEG_INF_FLAG:
            m_flag = other.m_flag;
            break;
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      case POS_INF_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
          case POS_INF_FLAG:
            break;
          case NEG_INF_FLAG:
            throw infinite_error("Indeterminate form: +inf + -inf");
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      case NEG_INF_FLAG:
        switch (other.m_flag) {
          case POS_INF_FLAG:
            throw infinite_error("Indeterminate form: -inf + +inf");
          case FINITE_FLAG:
          case NEG_INF_FLAG:
            break;
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      default:
        throw infinite_error("Internal error: invalid infinite flag.");
    }
    return *this;
  }

  Extended& operator-=(const Extended& other) {
    switch (m_flag) {
      case FINITE_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
            m_value = static_cast<T>(m_value - other.m_value);
            break;
          case POS_INF_FLAG:
          case NEG_INF_FLAG:
            m_flag = static_cast<char>(-other.m_flag);
            break;
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      case POS_INF_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
          case NEG_INF_FLAG:
            break;
          case POS_INF_FLAG:
            throw infinite_error("Indeterminate form: +inf - +inf");
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      case NEG_INF_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
          case POS_INF_FLAG:
            break;
          case NEG_INF_FLAG:
            throw infinite_error("Indeterminate form: -inf - -inf");
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      default:
        throw infinite_error("Internal error: invalid infinite flag.");
    }
    return *this;
  }

  Extended& operator*=(const Extended& other) {
    static constexpr T zero = static_cast<T>(0);
    switch (m_flag) {
      case FINITE_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
            m_value = static_cast<T>(m_value * other.m_value);
            break;
          case POS_INF_FLAG:
          case NEG_INF_FLAG:
            if (m_value < zero)
              m_flag = static_cast<char>(-other.m_flag);
            else if (m_value > zero)
              m_flag = other.m_flag;
            break;
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      case POS_INF_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
            if (other.m_value == zero) {
              m_value = 0;
              m_flag = FINITE_FLAG;
            } else if (other.m_value < zero) {
              m_flag = NEG_INF_FLAG;
            }
            break;
          case POS_INF_FLAG:
          case NEG_INF_FLAG:
            m_flag = other.m_flag;
            break;
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      case NEG_INF_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
            if (other.m_value == zero) {
              m_value = 0;
              m_flag = FINITE_FLAG;
            } else if (other.m_value < zero) {
              m_flag = POS_INF_FLAG;
            }
            break;
          case POS_INF_FLAG:
          case NEG_INF_FLAG:
            m_flag = static_cast<char>(-other.m_flag);
            break;
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      default:
        throw infinite_error("Internal error: invalid infinite flag.");
    }
    return *this;
  }

  Extended& operator/=(const Extended& other) {
    static constexpr T zero = static_cast<T>(0);
    switch (m_flag) {
      case FINITE_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
            inf_assert(other.m_value != zero, "Indeterminate form: +inf / 0");
            m_value = static_cast<T>(m_value / other.m_value);
            break;
          case POS_INF_FLAG:
          case NEG_INF_FLAG:
            m_value = zero;
            break;
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      case POS_INF_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
            inf_assert(other.m_value != zero, "Indeterminate form: +inf / 0");
            if (other.m_value < zero) m_flag = NEG_INF_FLAG;
            break;
          case POS_INF_FLAG:
            throw infinite_error("+inf / +inf indeterminate form.");
          case NEG_INF_FLAG:
            throw infinite_error("+inf / -inf indeterminate form.");
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      case NEG_INF_FLAG:
        switch (other.m_flag) {
          case FINITE_FLAG:
            inf_assert(other.m_value != zero, "Indeterminate form: +inf / 0");
            if (other.m_value < zero) m_flag = POS_INF_FLAG;
            break;
          case POS_INF_FLAG:
            throw infinite_error("-inf / +inf indeterminate form.");
          case NEG_INF_FLAG:
            throw infinite_error("-inf / -inf indeterminate form.");
          default:
            throw infinite_error("Internal error: invalid infinite flag.");
        }
        break;
      default:
        throw infinite_error("Internal error: invalid infinite flag.");
    }
    return *this;
  }

  Extended& operator%=(const Extended& other) {
    inf_assert(finite() && other.finite(),
               "Finite error: modular arithmetic requires finite values.");
    m_value = static_cast<T>(m_value % other.m_value);
    return *this;
  }

  // BITWISE

  Extended operator~() const {
    inf_assert(finite(), "Finite error: bitwise not requires finite values.");
    return Extended(static_cast<T>(~m_value));
  }

  Extended& operator&=(const Extended& other) {
    inf_assert(finite() && other.finite(),
               "Finite error: bitwise and requires finite values.");
    m_value = m_value & other.m_value;
    return *this;
  }

  Extended& operator|=(const Extended& other) {
    inf_assert(finite() && other.finite(),
               "Finite error: bitwise or requires finite values.");
    m_value |= other.m_value;
    return *this;
  }

  Extended& operator^=(const Extended& other) {
    inf_assert(finite() && other.finite(),
               "Finite error: bitwise xor requires finite values.");
    m_value ^= other.m_value;
    return *this;
  }

  Extended& operator<<=(const Extended& other) {
    inf_assert(finite() && other.finite(),
               "Finite error: bitwise leftshift requires finite values.");
    m_value = static_cast<T>(m_value << other.m_value);
    return *this;
  }

  Extended& operator>>=(const Extended& other) {
    inf_assert(finite() && other.finite(),
               "Finite error: bitwise rightshift requires finite values.");
    m_value = static_cast<T>(m_value >> other.m_value);
    return *this;
  }

  // STREAMS

  friend std::ostream& operator<<(std::ostream& os, const Extended& ext) {
    if (ext.finite()) {
      os << ext.m_value;
    } else {
      os << (ext.m_flag == POS_INF_FLAG ? "+inf" : "-inf");
    }
    return os;
  }

  /**
   * WARNING: only valid for finite values.
   */
  friend std::istream& operator>>(std::istream& is, Extended& ext) {
    ext.m_flag = FINITE_FLAG;
    return is >> ext.m_value;
  }
};

// SYMMETRIC ARITHMETIC AND BITWISE

template <typename T>
Extended<T> operator+(Extended<T> lhs, const Extended<T>& rhs) {
  lhs += rhs;
  return lhs;
}

template <typename T>
Extended<T> operator-(Extended<T> lhs, const Extended<T>& rhs) {
  lhs -= rhs;
  return lhs;
}

template <typename T>
Extended<T> operator*(Extended<T> lhs, const Extended<T>& rhs) {
  lhs *= rhs;
  return lhs;
}

template <typename T>
Extended<T> operator/(Extended<T> lhs, const Extended<T>& rhs) {
  lhs /= rhs;
  return lhs;
}

template <typename T>
Extended<T> operator%(Extended<T> lhs, const Extended<T>& rhs) {
  lhs %= rhs;
  return lhs;
}

template <typename T>
Extended<T> operator&(Extended<T> lhs, const Extended<T>& rhs) {
  lhs &= rhs;
  return lhs;
}

template <typename T>
Extended<T> operator|(Extended<T> lhs, const Extended<T>& rhs) {
  lhs |= rhs;
  return lhs;
}

template <typename T>
Extended<T> operator^(Extended<T> lhs, const Extended<T>& rhs) {
  lhs ^= rhs;
  return lhs;
}

template <typename T>
Extended<T> operator<<(Extended<T> lhs, const Extended<T>& rhs) {
  lhs <<= rhs;
  return lhs;
}

template <typename T>
Extended<T> operator>>(Extended<T> lhs, const Extended<T>& rhs) {
  lhs >>= rhs;
  return lhs;
}
