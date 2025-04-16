#pragma once

#include "DynNum.hpp"
#include <sstream>

class Conversion {
  public:
    static std::string string(const DynNum<>& input_num, int precision = -1) {
        std::stringstream result;

        if (input_num.sign)
            result << '-';

        int digits = (int)input_num.digits.size();
        if (precision > 0 && digits > precision) {
            digits = precision;
        }

        int trailing_zeroes = std::max(0, input_num.dot_index - digits);
        int leading_zeroes = input_num.get_leading_zeroes();
        int dot_move = 0;

        if (trailing_zeroes > 0)
            dot_move = -trailing_zeroes;
        if (leading_zeroes > 0)
            dot_move = leading_zeroes;

        int dot_index = input_num.dot_index + dot_move;

        if (dot_index > 0) {
            for (int i = 0; i < dot_index; i++) {
                result << input_num.digits[i];
            }
        } else {
            result << '0';
        }

        if (dot_index < digits) {
            result << '.';

            for (int i = dot_index; i < digits; i++) {
                result << input_num.digits[i];
            }
        }

        if (dot_move != 0) {
            result << "E" << -dot_move;
        }

        return result.str();
    }
};

template <typename T> std::string DynNum<T>::to_string(int precision) const {
    return Conversion::string(*this, precision);
}