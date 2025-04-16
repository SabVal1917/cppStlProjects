#pragma once

#include <bit>
#include <iostream>
#include <type_traits>
#include <vector>
#include <cstring>
#include <climits>

typedef unsigned __int128 uint128_t;

template <typename T> struct LargerType {};

template <typename T>
    requires(std::is_integral_v<T> && sizeof(T) == 1)
struct LargerType<T> {
    using type = uint16_t;
};

template <typename T>
    requires(std::is_integral_v<T> && sizeof(T) == 2)
struct LargerType<T> {
    using type = uint32_t;
};

template <typename T>
    requires(std::is_integral_v<T> && sizeof(T) == 4)
struct LargerType<T> {
    using type = uint64_t;
};

template <typename T>
    requires(std::is_integral_v<T> && sizeof(T) == 8)
struct LargerType<T> {
    using type = uint128_t;
};

template <int _digit_count, bool _signed = true, typename _Digit = unsigned char> struct StaticLongIntStorage {
    using Digit = _Digit;
    static constexpr bool is_static = true;
    static constexpr int get_digit_count() { return _digit_count; }
    static constexpr int get_signed() { return _signed; }
    static constexpr int get_digit_bitlength() { return sizeof(Digit) * CHAR_BIT; }

    static_assert(get_digit_count() > 0, "Cannot create LongInt with zero digits");

    constexpr Digit* get_digits() { return digits; }
    constexpr const Digit* get_digits() const { return digits; }

    Digit digits[get_digit_count()]{};
};

template <bool _is_signed = true, typename _Digit = unsigned char> struct DynamicLongIntStorage {
    using Digit = _Digit;
    static constexpr bool is_static = false;
    constexpr int get_digit_count() const { return digits.size(); }
    static constexpr int get_signed() { return _is_signed; }
    static constexpr int get_digit_bitlength() { return sizeof(Digit) * CHAR_BIT; }

    DynamicLongIntStorage(int size) : digits(size) {}

    Digit* get_digits() { return digits.data(); }
    const Digit* get_digits() const { return digits.data(); }

    std::vector<Digit> digits;
};

template <typename Storage> class LongInt {
  public:
    using Digit = typename Storage::Digit;

    static constexpr int get_digit_count()
        requires(Storage::is_static)
    {
        return Storage::get_digit_count();
    }
    constexpr int get_digit_count() const
        requires(!Storage::is_static)
    {
        return storage.get_digit_count();
    }
    static constexpr int get_signed() { return Storage::get_signed(); }
    static constexpr int get_digit_bitlength() { return Storage::get_digit_bitlength(); }

    constexpr LongInt()
        requires Storage::is_static
    = default;

    static LongInt with_digits(int digits)
        requires(!Storage::is_static)
    {
        return LongInt(digits, std::false_type());
    }

    static LongInt as(const LongInt& other)
        requires(!Storage::is_static)
    {
        return with_digits(other.storage.get_digit_count());
    }

    static LongInt as(const LongInt& other)
        requires(Storage::is_static)
    {
        return LongInt();
    }

    constexpr LongInt(const LongInt& other) : storage(other.storage) { *this = other; }

    template <typename T>
        requires(std::is_integral_v<T> && Storage::is_static)
    constexpr LongInt(const T& other) {
        *this = other;
    }

    template <typename T>
        requires(std::is_integral_v<T> && !Storage::is_static)
    constexpr LongInt(int digits, const T& other) : storage(digits) {
        *this = other;
    }

    template <typename T> constexpr LongInt& operator=(const T& other) {
        int i = 0;
        for (; i < get_digit_count() && i * get_digit_bitlength() < sizeof(other) * CHAR_BIT; ++i) {
            get_digits()[i] = other >> i * get_digit_bitlength();
        }
        for (; i < get_digit_count(); i++) {
            get_digits()[i] = 0;
        }
        return *this;
    }

    constexpr LongInt& operator+=(const LongInt& other) {
        unsigned int carry = 0;
        auto max = (Digit)-1;
        for (int i = 0; i < get_digit_count(); ++i) {
            Digit sum = get_digits()[i] + other.get_digits()[i] + carry;
            carry = (get_digits()[i] > max - other.get_digits()[i]) ||
                    (carry && get_digits()[i] == max - other.get_digits()[i]);
            get_digits()[i] = sum;
        }
        return *this;
    }

    constexpr LongInt operator+(const LongInt& other) const {
        LongInt result = *this;
        result += other;
        return result;
    }

    constexpr LongInt& operator-=(const LongInt& other) {
        unsigned int borrow = 0;
        for (int i = 0; i < get_digit_count(); ++i) {
            Digit diff = get_digits()[i] - other.get_digits()[i] - borrow;
            borrow = other.get_digits()[i] > get_digits()[i] || (other.get_digits()[i] == get_digits()[i] && borrow);
            get_digits()[i] = diff;
        }
        return *this;
    }

    constexpr LongInt operator-(const LongInt& other) const {
        LongInt result = *this;
        result -= other;
        return result;
    }

    constexpr LongInt operator-() const {
        LongInt result = LongInt::as(*this);
        for (int i = 0; i < get_digit_count(); ++i) {
            result.get_digits()[i] = ~get_digits()[i];
        }
        ++result;
        return result;
    }

    constexpr LongInt operator*(const LongInt& other) const {
        auto result = *this;
        result *= other;
        return result;
    }

    constexpr LongInt& operator*=(const LongInt& other) {
        LongInt a = is_negative() ? -*this : *this;
        LongInt b = other.is_negative() ? -other : other;
        LongInt result = LongInt::as(*this);
        LongInt tmp = LongInt::as(*this);

        typedef typename LargerType<Digit>::type LargeType;
        for (int i = 0; i < get_digit_count(); ++i) {
            for (int j = 0; j < get_digit_count(); ++j) {
                tmp = (LargeType)a.get_digits()[i] * (LargeType)b.get_digits()[j];
                tmp <<= (i + j) * get_digit_bitlength();
                result += tmp;
            }
        }
        if (get_signed() && is_negative() != other.is_negative()) {
            result = -result;
        }
        *this = result;
        return *this;
    }

    constexpr LongInt& operator<<=(int shift) {
        assert(shift >= 0);
        if (shift == 0) {
            return *this;
        }

        int shift_digits = shift / get_digit_bitlength();
        if (shift_digits >= get_digit_count()) {
            *this = 0;
            return *this;
        }
        int shift_bits = shift % get_digit_bitlength();

        for (int i = get_digit_count() - 1; i >= shift_digits; i--) {
            Digit low = get_digits()[i - shift_digits];

            get_digits()[i] = low << shift_bits;

            if (i - shift_digits - 1 >= 0 && i - shift_digits - 1 < get_digit_count() && shift_bits != 0) {
                Digit high = get_digits()[i - shift_digits - 1];
                get_digits()[i] |= high >> (get_digit_bitlength() - shift_bits);
            }
        }

        for (int i = 0; i < shift_digits; i++) {
            get_digits()[i] = 0;
        }

        return *this;
    }

    constexpr LongInt& operator>>=(int shift) {
        assert(shift >= 0);
        if (shift == 0) {
            return *this;
        }

        int shift_digits = shift / get_digit_bitlength();
        if (shift_digits >= get_digit_count()) {
            *this = 0;
            return *this;
        }
        int shift_bits = shift % get_digit_bitlength();

        for (int i = 0; i < get_digit_count() - shift_digits; i++) {
            Digit low = get_digits()[i + shift_digits];

            if (get_signed() && i + shift_digits == get_digit_count() - 1) {
                get_digits()[i] = (Digit)((typename std::make_signed<Digit>::type)(low) >> shift_bits);
            } else {
                get_digits()[i] = low >> shift_bits;
            }

            if (i < get_digit_count() - shift_digits - 1) {
                Digit high = get_digits()[i + shift_digits + 1];
                get_digits()[i] |= high << (get_digit_bitlength() - shift_bits);
            }
        }

        if (is_negative()) {
            for (int i = get_digit_count() - shift_digits; i < get_digit_count(); i++) {
                get_digits()[i] = (Digit)-1;
            }
        } else {
            for (int i = get_digit_count() - shift_digits; i < get_digit_count(); i++) {
                get_digits()[i] = 0;
            }
        }

        return *this;
    }

    constexpr LongInt& operator&=(const LongInt& other) {
        for (int i = 0; i < get_digit_count(); ++i) {
            get_digits()[i] &= other.get_digits()[i];
        }
        return *this;
    }

    constexpr LongInt& operator|=(const LongInt& other) {
        for (int i = 0; i < get_digit_count(); ++i) {
            get_digits()[i] |= other.get_digits()[i];
        }
        return *this;
    }

    constexpr LongInt& operator^=(const LongInt& other) {
        for (int i = 0; i < get_digit_count(); ++i) {
            get_digits()[i] ^= other.get_digits()[i];
        }
        return *this;
    }

    constexpr LongInt operator~() const {
        LongInt result;
        for (int i = 0; i < get_digit_count(); ++i) {
            result.get_digits()[i] = ~get_digits()[i];
        }
        return result;
    }

    constexpr LongInt operator<<(int shift) const {
        auto result = *this;
        result <<= shift;
        return result;
    }

    constexpr LongInt operator>>(int shift) const {
        auto result = *this;
        result >>= shift;
        return result;
    }

    constexpr LongInt operator|(const LongInt& other) const {
        auto result = *this;
        result |= other;
        return result;
    }

    constexpr LongInt operator&(const LongInt& other) const {
        auto result = *this;
        result &= other;
        return result;
    }

    constexpr LongInt& operator++() {
        *this += (as(*this) = 1);
        return *this;
    }

    constexpr LongInt& operator--() {
        *this -= (as(*this) = 1);
        return *this;
    }

    constexpr LongInt operator++(int) {
        auto result = *this;
        *this += (as(*this) = 1);
        return result;
    }

    constexpr LongInt operator--(int) {
        auto result = *this;
        *this -= (as(*this) = 1);
        return result;
    }

    constexpr bool operator==(const LongInt& other) const {
        for (int i = 0; i < get_digit_count(); ++i) {
            if (get_digits()[i] != other.get_digits()[i])
                return false;
        }
        return true;
    }

    constexpr bool operator!=(const LongInt& other) const { return !(*this == other); }

    constexpr bool operator<(const LongInt& other) const {
        if (get_signed() && is_negative() != other.is_negative()) {
            return is_negative();
        }
        for (int i = get_digit_count() - 1; i >= 0; --i) {
            if (get_digits()[i] < other.get_digits()[i])
                return true;
            if (get_digits()[i] > other.get_digits()[i])
                return false;
        }
        return false;
    }

    constexpr bool operator<=(const LongInt& other) const { return *this < other || *this == other; }

    constexpr bool operator>(const LongInt& other) const { return !(*this <= other); }

    constexpr bool operator>=(const LongInt& other) const { return !(*this < other); }

    constexpr bool is_even() const { return (get_digits()[0] & 1) == 0; }

    constexpr bool is_negative() const {
        return get_signed() && get_digits()[get_digit_count() - 1] >> (get_digit_bitlength() - 1);
    }

    constexpr int countl_zero() const {
        for (int i = get_digit_count() - 1; i >= 0; i--) {
            int zero = std::countl_zero(get_digits()[i]);
            if (zero < get_digit_bitlength()) {
                return (get_digit_count() - 1 - i) * get_digit_bitlength() + zero;
            }
        }
        return get_digit_bitlength() * get_digit_count();
    }

    constexpr bool is_zero() const {
        for (int i = 0; i < get_digit_count(); i++) {
            if (get_digits()[i] != 0)
                return false;
        }
        return true;
    }

    constexpr explicit operator unsigned long long() const {
        unsigned long long result = 0;
        memcpy((void*)&result, (void*)get_digits(),
               std::min(sizeof(result), (size_t)get_digit_count() * get_digit_bitlength() / CHAR_BIT));
        if (is_negative()) {
            for (int i = get_digit_count() * get_digit_bitlength() / CHAR_BIT; i < sizeof(unsigned long long); i++) {
                result |= ((unsigned long long)(Digit)-1) << i * 8;
            }
        }
        return result;
    }

    constexpr long long to_int() const { return (long long)(unsigned long long)*this; }

    constexpr unsigned long long to_uint() const { return (unsigned long long)*this; }

    constexpr Digit* get_digits() { return storage.get_digits(); }
    constexpr const Digit* get_digits() const { return storage.get_digits(); }

    Storage storage;

  private:
    constexpr LongInt(int digits, std::false_type nothing)
        requires(!Storage::is_static)
        : storage(digits) {}
};

template <int _digit_count, bool _signed = true, typename Digit = unsigned char>
using StaticLongInt = LongInt<StaticLongIntStorage<_digit_count, _signed, Digit>>;

template <bool _signed = true, typename Digit = unsigned char>
using DynamicLongInt = LongInt<DynamicLongIntStorage<_signed, Digit>>;
