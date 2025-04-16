#pragma once
#include <cctype>
#include <deque>
#include <cassert>

template <int _radix = 10> struct Digit {
    static constexpr int radix = _radix;
    int value;

    explicit Digit(int value) : value((value % radix + radix) % radix) {}

    Digit& operator+=(const Digit& other) {
        value = (value + other.value) % radix;
        return *this;
    }

    Digit& operator-=(const Digit& other) {
        value = (value - other.value + radix) % radix;
        return *this;
    }

    Digit operator+(const Digit& other) const {
        Digit result = *this;
        result += other;
        return result;
    }

    Digit operator-(const Digit& other) const {
        Digit result = *this;
        result -= other;
        return result;
    }

    Digit& operator*=(Digit& other) {
        value = (value * other.value) % radix;
        return *this;
    }

    Digit& operator/=(Digit& other) {
        value /= other.value;
        return *this;
    }

    Digit operator*(Digit& other) const {
        Digit result = *this;
        result *= other;
        return result;
    }

    Digit operator/(Digit& other) const {
        Digit result = *this;
        result /= other;
        return result;
    }

    bool add(const Digit& other, bool carry = false) {
        bool new_carry = value + other.value + (int)carry >= radix;
        *this += other + Digit((int)carry);
        return new_carry;
    }

    bool sub(const Digit& other, bool carry = false) {
        bool new_carry = other.value + (int)carry > value;
        *this -= other + Digit((int)carry);
        return new_carry;
    }

    auto operator<=>(const Digit& other) const { return value <=> other.value; }

    bool operator==(const Digit& other) const = default;
};

template <typename T, int radix> T& operator<<(T& stream, const Digit<radix>& digit) {
    if (digit.value < 10) {
        stream << digit.value;
    } else if (digit.value < 36) {
        stream << (char)('A' + digit.value - 10);
    } else {
        stream << '<' << digit.value << '>';
    }
    return stream;
}

template <typename Digit = Digit<>> class DynNum {
  public:
    DynNum() = default;

    explicit DynNum(const char* number) {
        if (!parse_number(number)) {
            *this = {};
        }
    }

    DynNum(bool sign, int dot_index, std::deque<Digit> digits)
        : sign(sign), dot_index(dot_index), digits(std::move(digits)) {}

    static DynNum one;

    bool operator==(const DynNum& other) const {
        return sign == other.sign && dot_index == other.dot_index && digits == other.digits;
    }

    bool operator!=(const DynNum& other) const { return !(*this == other); }

    bool operator>(const DynNum& other) const {
        if (other.is_zero()) {
            if (is_zero()) {
                return false;
            }
            return !sign;
        }

        if (is_zero()) {
            return other.sign;
        }

        if (sign != other.sign) {
            return !sign;
        }

        if (dot_index != other.dot_index) {
            return (dot_index > other.dot_index) ^ sign;
        }

        for (int i = 0; i < digits.size() || i < other.digits.size(); i++) {
            Digit a = i < digits.size() ? digits[i] : Digit(0);
            Digit b = i < other.digits.size() ? other.digits[i] : Digit(0);

            if (a != b) {
                return (a > b) ^ sign;
            }
        }

        return false;
    }

    DynNum& operator+=(const DynNum& b) {
        DynNum& a = *this;
        if (is_zero()) {
            a = b;
            return a;
        }

        if (b.is_zero())
            return a;

        if (a.sign != b.sign) {
            a.sign = !sign;
            bool swap = (*this < b) ^ a.sign;
            a.sign = !b.sign;
            if (swap) {
                auto result = b;
                result += *this;
                *this = result;
                return *this;
            }
        }

        int max_digit_index = std::max(a.dot_index, b.dot_index);
        int min_digit_index = std::min(a.dot_index - (int)a.digits.size(), b.dot_index - (int)b.digits.size());

        // Prepare digits

        while (a.dot_index < max_digit_index) {
            a.digits.push_front(Digit(0));
            a.dot_index++;
        }

        while (min_digit_index < a.dot_index - (int)a.digits.size()) {
            a.digits.push_back(Digit(0));
        }

        // Invariants:
        // - a.sign == b.sign || abs(a) >= abs(b)
        // - a.dot_index >= b.dot_index
        // - a.digits.size() >= b.digits.size()
        // - a digits cover b digits

        bool carry = false;
        for (int i = b.digits.size() - 1; i >= 0; i--) {
            int a_index = i + a.dot_index - b.dot_index;
            if (a.sign == b.sign) {
                carry = a.digits[a_index].add(b.digits[i], carry);
            } else {
                carry = a.digits[a_index].sub(b.digits[i], carry);
            }
        }

        if (carry) {
            for (int i = a.dot_index - b.dot_index - 1; i >= 0 && carry; i--) {
                if (a.sign == b.sign) {
                    carry = a.digits[i].add(Digit(0), carry);
                } else {
                    carry = a.digits[i].sub(Digit(0), carry);
                }
            }
        }

        if (carry) {
            assert(a.sign == b.sign);
            a.digits.push_front(Digit(1));
            a.dot_index++;
        }

        normalize();
        return *this;
    }

    DynNum partition(int begin, int end) const {
        DynNum result;
        begin = std::max(0, begin);
        end = std::min(end, (int)digits.size());
        if (begin < 0 && end < 0 || begin >= end)
            return result;

        for (int i = begin; i < end; i++) {
            result.digits.push_back(digits[i]);
        }
        result.dot_index = result.digits.size();
        result.normalize();

        return result;
    }

    static DynNum square_mul(const DynNum& a, const DynNum& b) {
        if (a.is_zero() || b.is_zero()) {
            return DynNum();
        }

        DynNum result;
        DynNum tmp;

        for (int i = 0; i < b.digits.size(); i++) {
            for (int j = 0; j < a.digits.size(); j++) {
                auto power = b.dot_index - i + a.dot_index - j;
                int product = b.digits[i].value * a.digits[j].value;

                tmp.digits.clear();
                tmp.digits.push_back(Digit(product / Digit::radix));
                tmp.digits.push_back(Digit(product));
                tmp.dot_index = power;
                tmp.normalize();
                result += tmp;
            }
        }

        result.sign = a.sign ^ b.sign;
        result.normalize();
        return result;
    }

    static DynNum karatsuba_mul(const DynNum& a, const DynNum& b) {
        if (a.is_zero() || b.is_zero())
            return {};
        if (a == one)
            return b;
        if (b == one)
            return a;

        if (a.digits.size() == 1 && b.digits.size() == 1) {
            int product = a.digits[0].value * b.digits[0].value;
            DynNum result = {
                (bool)(a.sign ^ b.sign), a.dot_index + b.dot_index, {Digit(product / Digit::radix), Digit(product)}};
            result.normalize();
            return result;
        }

        int size = std::max(a.digits.size(), b.digits.size());
        int partition_l_size = size / 2;
        int partition_r_size = (size + 1) / 2;

        int partition_a = a.digits.size() - partition_r_size;
        int partition_b = b.digits.size() - partition_r_size;

        DynNum al = a.partition(partition_a - partition_l_size, partition_a);
        DynNum ar = a.partition(partition_a, partition_a + partition_r_size);

        DynNum bl = b.partition(partition_b - partition_l_size, partition_b);
        DynNum br = b.partition(partition_b, partition_b + partition_r_size);

        DynNum P1 = al * bl;
        DynNum P2 = ar * br;

        al += ar;
        bl += br;

        DynNum P3 = al * bl;

        P3 -= P1;
        P3 -= P2;

        P1.dot_index += 2 * partition_r_size;
        P3.dot_index += partition_r_size;

        P1 += P2;
        P1 += P3;

        P1.dot_index += a.dot_index + b.dot_index - a.digits.size() - b.digits.size();
        P1.sign = a.sign ^ b.sign;
        P1.normalize();
        return P1;
    }

    DynNum operator*(const DynNum& other) {
        if (digits.size() * other.digits.size() > 500) {
            return karatsuba_mul(*this, other);
        }
        return square_mul(*this, other);
    }

    DynNum& operator*=(const DynNum& other) {
        *this = *this * other;
        return *this;
    }

    DynNum operator+(const DynNum& other) const {
        DynNum result = *this;
        result += other;
        return result;
    }

    DynNum& operator-=(const DynNum& other) {
        *this += -other;
        return *this;
    }

    DynNum operator-(const DynNum& other) const {
        DynNum result = *this;
        result -= other;
        return result;
    }

    int get_trailing_zeroes() const { return std::max(0, dot_index - (int)digits.size()); }

    int get_leading_zeroes() const { return std::max(0, -dot_index); }

    bool operator<(const DynNum& other) const { return other > *this; }

    bool operator<=(const DynNum& other) const { return !(*this > other); }

    bool operator>=(const DynNum& other) const { return !(other > *this); }

    DynNum operator-() const { return {!sign, dot_index, digits}; }

    bool is_zero() const { return digits.size() == 0; }

    DynNum& binpow(int power) {
        assert(power >= 0);

        DynNum res(false, 1, {Digit(1)});

        while (power) {
            if (power & 1) {
                res *= *this;
                --power;
            } else {
                *this *= *this;
                power >>= 1;
            }
        }
        *this = res;
        return *this;
    }

    void normalize() {
        while (digits.size() > 0 && digits.front() == Digit(0)) {
            digits.pop_front();
            dot_index--;
        }

        while (digits.size() > 0 && digits.back() == Digit(0)) {
            digits.pop_back();
        }

        if (is_zero()) {
            sign = false;
            dot_index = 0;
        }
    }

    std::string to_string(int precision = -1) const;

    bool sign = false;
    int dot_index = 0;
    std::deque<Digit> digits{};

  private:
    int char_to_digit_radix(char c) {
        if (c < '0')
            return -1;

        if (Digit::radix <= 10) {
            if (c > '0' + (Digit::radix - 1))
                return -1;
        }

        if (c <= '9')
            return c - '0';

        if (Digit::radix <= 16) {
            c = (signed char)tolower(c);
            if (c < 'a')
                return -1;
            if (c > 'a' + (Digit::radix - 11))
                return -1;
            return c - 'a' + 10;
        }

        return -1;
    }

    bool parse_number(const char* number) {
        const char* p = number;
        while (isspace(*p))
            p++;

        if (*p == '-') {
            sign = true;
            p++;
        }

        char c = '\0';
        int radix = Digit::radix;

        int integer_digits = parse_digits_lexeme(p, digits);
        if (integer_digits == 0 && *p != '.') {
            return false;
        }

        dot_index = integer_digits;

        p += integer_digits;
        c = *p;
        if (c == '.') {
            p++;
            int parsed = parse_digits_lexeme(p, digits);
            if (parsed == 0) {
                return false;
            }
            p += parsed;
        }

        c = *p;
        if (c == 'e' || c == 'E') {
            p++;
            c = *p;
            int exponent_digit = 1;
            if (c == '+' || c == '-') {
                if (c == '-')
                    exponent_digit = -1;
                p++;
            }

            std::deque<Digit> exponent_digits;

            if (parse_digits_lexeme(p, exponent_digits) == 0) {
                return false;
            }

            for (int i = exponent_digits.size() - 1; i >= 0; i--) {
                dot_index += exponent_digit * exponent_digits[i].value;
                exponent_digit *= radix;
            }
        }

        normalize();

        return true;
    }

    int parse_digits_lexeme(const char* p, std::deque<Digit>& digits) {
        int digit_count = 0;
        unsigned char digit = '\0';

        while (true) {
            digit = char_to_digit_radix(*p);
            if (digit == (unsigned char)-1)
                break;
            p++;
            digits.push_back(Digit(digit));
            digit_count++;
        }

        return digit_count;
    }
};

template <typename T> DynNum<T> DynNum<T>::one = DynNum(false, 1, {Digit(1)});
