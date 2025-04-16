#include "ieee754_clf.h"

#include <stdint.h>
#include <string.h>

int get_most_significant_bit(unsigned char value) { return (value >> 7) & 1; }

float_class_t classify(double x) {
  union {
    double number;
    uint8_t bytes[8];
  } u;
  u.number = x;

  if (u.bytes[7] == 17 * 15 && u.bytes[6] == 16 * 15 + 8) {
    return NaN;
  }

  if (u.bytes[7] == 17 * 15 && u.bytes[6] == 16 * 15) {
    return MinusInf;
  }

  if (u.bytes[7] == 16 * 7 + 15 && u.bytes[6] == 16 * 15) {
    return Inf;
  }

  uint8_t is_zero = 1;
  for (int i = 0; i < 7; ++i) {
    if (u.bytes[i] > 0) {
      is_zero = 0;
      break;
    }
  }
  union {
    double number;
    uint64_t u;
    struct {
      uint64_t mantissa : 52;
      uint64_t exponent : 11;
      uint64_t sign : 1;
    } parts;
  } du;
  du.number = x;
  if (du.parts.mantissa != 0 && du.parts.exponent == 0) {
    if (get_most_significant_bit(u.bytes[7]) == 1) {
      return MinusDenormal;
    }
    return Denormal;
  }

  if (get_most_significant_bit(u.bytes[7]) == 1) {
    if (is_zero) {
      return MinusZero;
    }
    return MinusRegular;
  } else {
    if (is_zero) {
      return Zero;
    }
    return Regular;
  }
  return NaN;
}
