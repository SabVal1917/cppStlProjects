#include <iostream>
#include <string>
#include <vector>

class BigInteger {
 private:
  std::vector<long long> num_;
  bool isNegative_;
  static const long long kBase = 10'000'000;
  static const size_t kLenOfBase = 7;

  void parseShort(long long val);
  void deleteLeadZeroes();

  void sumForSameSign(const BigInteger& other);
  void sumForDiffSign(const BigInteger& other);

  void checkSign(const std::string& new_val);
  void transformString(const std::string& new_val);
  void putStringIntoNumber(size_t size_val, const std::string& str);

  void shift(size_t sizeShift);

 public:
  BigInteger();
  BigInteger(long long new_val);
  BigInteger(const std::string& new_val);
  std::string toString() const;
  size_t getSize() const;
  bool getSign() const;
  const std::vector<long long>& getVect() const;

  BigInteger operator-() const;
  BigInteger& operator--();
  BigInteger operator--(int);
  BigInteger& operator++();
  BigInteger operator++(int);

  BigInteger& operator+=(const BigInteger& other);
  BigInteger& operator-=(const BigInteger& other);
  BigInteger& operator*=(const BigInteger& other);
  BigInteger& operator/=(const BigInteger& other);
  BigInteger& operator%=(const BigInteger& other);

  explicit operator bool() const;
};

bool operator<(const BigInteger& left, const BigInteger& right);
bool operator>(const BigInteger& left, const BigInteger& right);
bool operator<=(const BigInteger& left, const BigInteger& right);
bool operator>=(const BigInteger& left, const BigInteger& right);
bool operator==(const BigInteger& left, const BigInteger& right);
bool operator!=(const BigInteger& left, const BigInteger& right);

void BigInteger::transformString(const std::string& new_val) {
  size_t size_val = new_val.size();
  if (isNegative_) {
    std::string tmp = new_val.substr(1, size_val - 1);
    --size_val;
    putStringIntoNumber(size_val, tmp);
  } else {
    putStringIntoNumber(size_val, new_val);
  }
}

std::string BigInteger::toString() const {
  if (num_.empty()) {
    return "0";
  }
  std::string result = (isNegative_ ? "-" : "") + std::to_string(num_.back());
  for (size_t i = num_.size(); i >= 2; --i) {
    std::string tmp = std::to_string(num_[i - 2]);
    if (tmp.size() != kLenOfBase) {
      while (tmp.size() < kLenOfBase) {
        tmp = '0' + tmp;
      }
    }
    result += tmp;
  }
  return result;
}

void BigInteger::putStringIntoNumber(size_t size_val, const std::string& str) {
  if (size_val == 0) {
    return;
  }
  while (size_val >= kLenOfBase) {
    num_.push_back(std::stoi(str.substr(size_val - kLenOfBase, kLenOfBase)));
    size_val -= kLenOfBase;
  }
  if (size_val > 0) {
    num_.push_back(std::stoi(str.substr(0, size_val)));
  }
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if (other.num_.empty()) {
    return *this;
  }
  if (num_.empty()) {
    *this = other;
    return *this;
  }
  if (isNegative_ == other.isNegative_) {
    sumForSameSign(other);
  } else {
    sumForDiffSign(other);
  }
  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
  *this += -other;
  return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  BigInteger result;
  result.num_.resize(num_.size() + other.getSize() + 3, 0);
  for (size_t i = 0; i < num_.size(); ++i) {
    for (size_t j = 0; j < other.getSize(); ++j) {
      result.num_[i + j] += (num_[i] * other.num_[j]);
    }
  }
  for (size_t i = 0; i < result.getSize(); ++i) {
    result.num_[i + 1] += result.num_[i] / kBase;
    result.num_[i] %= kBase;
  }
  result.isNegative_ = (isNegative_ != other.getSign());
  *this = result;
  deleteLeadZeroes();
  return *this;
}

BigInteger operator+(const BigInteger& left, const BigInteger& right) {
  BigInteger tmp = left;
  tmp += right;
  return tmp;
}

BigInteger operator-(const BigInteger& left, const BigInteger& right) {
  BigInteger tmp = left;
  tmp -= right;
  return tmp;
}

BigInteger operator*(const BigInteger& left, const BigInteger& right) {
  BigInteger tmp = left;
  tmp *= right;
  return tmp;
}

BigInteger abs(const BigInteger& val) {
  if (val.getSign()) {
    return -val;
  }
  return val;
}

void BigInteger::shift(size_t sizeShift) {
  num_.resize(num_.size() + sizeShift);

  for (size_t i = num_.size(); i >= sizeShift + 1; --i) {
    num_[i - 1] = num_[i - sizeShift - 1];
  }
  for (size_t i = sizeShift; i >= 1; --i) {
    num_[i - 1] = 0;
  }
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
  if (abs(*this) < abs(other)) {
    *this = 0;
    return *this;
  }

  BigInteger result;
  BigInteger carry;
  BigInteger residual(*this);
  BigInteger divider(other);
  residual.isNegative_ = false;
  divider.isNegative_ = false;
  result.num_.resize(num_.size() + 1);

  for (size_t i = num_.size(); i >= 1; --i) {
    long long left = 0;
    long long right = kBase;
    while (left + 1 < right) {
      long long mid = (right + left) / 2;
      carry = divider * mid;
      carry.shift(i - 1);
      if (carry > residual) {
        right = mid;
      } else {
        left = mid;
      }
    }

    result.num_[i - 1] = left;
    carry = divider * left;
    carry.shift(i - 1);
    residual -= carry;
  }

  result.deleteLeadZeroes();
  result.isNegative_ = (isNegative_ != other.isNegative_);
  *this = result;
  deleteLeadZeroes();
  return *this;
}

BigInteger operator/(const BigInteger& left, const BigInteger& right) {
  BigInteger tmp = left;
  tmp /= right;
  return tmp;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  *this -= (*this / other) * other;
  deleteLeadZeroes();
  return *this;
}

BigInteger operator%(const BigInteger& left, const BigInteger& right) {
  BigInteger tmp = left;
  tmp %= right;
  return tmp;
}

void BigInteger::sumForSameSign(const BigInteger& other) {
  num_.resize(std::max(other.getSize(), num_.size()));
  for (size_t i = 0; i < other.getSize(); ++i) {
    num_[i] += other.num_[i];
    if (num_[i] >= kBase) {
      if (i == num_.size() - 1) {
        num_.push_back(1);
        num_[i] -= kBase;
        break;
      }
      ++num_[i + 1];
      num_[i] -= kBase;
    }
  }

  for (size_t i = 0; i < num_.size(); ++i) {
    if (num_[i] >= kBase) {
      if (i == num_.size() - 1) {
        num_.push_back(1);
        num_[i] -= kBase;
        break;
      }
      ++num_[i + 1];
      num_[i] -= kBase;
    }
  }
}

void BigInteger::sumForDiffSign(const BigInteger& other) {
  BigInteger copy = other;
  if (abs(copy) == abs(*this)) {
    num_.clear();
    isNegative_ = false;
    return;
  }

  bool condition = abs(copy) < abs(*this);
  if (!condition) {
    BigInteger tmp = *this;
    std::swap(copy, *this);
    copy = tmp;
  }

  num_.resize(std::max(num_.size(), copy.num_.size()) + 1);
  for (size_t i = 0; i < copy.num_.size(); ++i) {
    num_[i] -= copy.num_[i];
    if (num_[i] < 0) {
      num_[i] += kBase;
      num_[i + 1] -= 1;
    }
  }

  for (size_t i = copy.num_.size(); i + 1 < num_.size(); ++i) {
    if (num_[i] < 0) {
      num_[i] += kBase;
      num_[i + 1] -= 1;
    } else {
      break;
    }
  }
  deleteLeadZeroes();
}

void BigInteger::parseShort(long long val) {
  if (val == 0) {
    return;
  }
  while (val > 0) {
    num_.push_back(val % kBase);
    val /= kBase;
  }
}

void BigInteger::deleteLeadZeroes() {
  while (!num_.empty() && num_.back() == 0) {
    num_.pop_back();
  }
  if (num_.empty()) {
    isNegative_ = false;
  }
}

BigInteger::BigInteger(const std::string& new_val) {
  checkSign(new_val);
  transformString(new_val);
  deleteLeadZeroes();
}

BigInteger BigInteger::operator-() const {
  BigInteger copy = *this;
  copy.isNegative_ = !isNegative_;
  return copy;
}

BigInteger& BigInteger::operator--() {
  *this -= 1;
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger tmp(*this);
  *this -= 1;
  return tmp;
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger tmp(*this);
  *this += 1;
  return tmp;
}

void BigInteger::checkSign(const std::string& new_val) {
  isNegative_ = (new_val[0] == '-');
}

BigInteger::BigInteger(long long new_val) : isNegative_(new_val < 0) {
  parseShort(std::abs(new_val));
}

size_t BigInteger::getSize() const { return num_.size(); }

bool BigInteger::getSign() const { return isNegative_; }

const std::vector<long long>& BigInteger::getVect() const { return num_; }

BigInteger::operator bool() const { return num_.size() != 0; }

BigInteger::BigInteger() : num_({0}), isNegative_(false) {}

bool operator<(const BigInteger& left, const BigInteger& right) {
  if (left.getSign() != right.getSign()) {
    return left.getSign();
  }

  if (left.getSize() < right.getSize()) {
    return !left.getSign();
  }

  if (left.getSize() > right.getSize()) {
    return left.getSign();
  }

  for (size_t i = left.getSize(); i >= 1; --i) {
    size_t index = i - 1;
    if (left.getVect()[index] < right.getVect()[index]) {
      return !left.getSign();
    }
    if (left.getVect()[index] > right.getVect()[index]) {
      return left.getSign();
    }
  }
  return false;
}

bool operator>(const BigInteger& left, const BigInteger& right) {
  return right < left;
}

bool operator<=(const BigInteger& left, const BigInteger& right) {
  return !(left > right);
}

bool operator>=(const BigInteger& left, const BigInteger& right) {
  return !(left < right);
}

bool operator==(const BigInteger& left, const BigInteger& right) {
  if (left.getSign() != right.getSign()) {
    return false;
  }
  return (left.getVect() == right.getVect());
}

bool operator!=(const BigInteger& left, const BigInteger& right) {
  return !(left == right);
}

std::ostream& operator<<(std::ostream& os, const BigInteger& big_int_outp) {
  os << big_int_outp.toString();
  return os;
}

std::istream& operator>>(std::istream& is, BigInteger& big_int_inp) {
  std::string input_string;
  is >> input_string;
  big_int_inp = BigInteger(input_string);
  return is;
}

BigInteger operator""_bi(const char* val) { return BigInteger(val); }

class Rational {
 private:
  BigInteger numerator_;
  BigInteger denominator_;
  BigInteger greaterCommonDivisor(const BigInteger& a, const BigInteger& b);

 public:
  Rational();
  Rational(const BigInteger& val);
  Rational(const BigInteger& num, const BigInteger& denom);
  Rational(long long int_val);
  explicit operator bool() const;
  Rational operator-() const;

  void checkSigns();
  void makeIrreducible();

  Rational& operator+=(const Rational& other);
  Rational& operator-=(const Rational& other);
  Rational& operator*=(const Rational& other);
  Rational& operator/=(const Rational& other);
  BigInteger getNumerator() const;
  BigInteger getDenominator() const;
  std::string toString();
  std::string asDecimal(size_t precision = 0) const;
  explicit operator double() const;
};

Rational& Rational::operator+=(const Rational& other) {
  numerator_ =
      numerator_ * other.denominator_ + other.numerator_ * denominator_;
  denominator_ *= other.denominator_;
  makeIrreducible();
  return *this;
}

Rational& Rational::operator-=(const Rational& other) {
  numerator_ =
      numerator_ * other.denominator_ - other.numerator_ * denominator_;
  denominator_ *= other.denominator_;
  makeIrreducible();
  return *this;
}

Rational& Rational::operator*=(const Rational& other) {
  numerator_ *= other.numerator_;
  denominator_ *= other.denominator_;
  makeIrreducible();
  return *this;
}

Rational& Rational::operator/=(const Rational& other) {
  numerator_ *= other.denominator_;
  denominator_ *= other.numerator_;
  makeIrreducible();
  return *this;
}

Rational operator+(const Rational& left, const Rational& right) {
  Rational tmp = left;
  tmp += right;
  return tmp;
}

Rational operator-(const Rational& left, const Rational& right) {
  Rational tmp = left;
  tmp -= right;
  return tmp;
}

Rational operator*(const Rational& left, const Rational& right) {
  Rational tmp = left;
  tmp *= right;
  return tmp;
}

Rational operator/(const Rational& left, const Rational& right) {
  Rational tmp = left;
  tmp /= right;
  return tmp;
}

bool operator<(const Rational& left, const Rational& right) {
  BigInteger tmp1 = left.getNumerator() * right.getDenominator();
  BigInteger tmp2 = left.getDenominator() * right.getNumerator();
  return (tmp1 < tmp2);
}

bool operator>(const Rational& left, const Rational& right) {
  return (right < left);
}

bool operator<=(const Rational& left, const Rational& right) {
  return !(left > right);
}

bool operator>=(const Rational& left, const Rational& right) {
  return !(left < right);
}

bool operator==(const Rational& left, const Rational& right) {
  return (left.getNumerator() == right.getNumerator()) &&
         (left.getDenominator() == right.getDenominator());
}

bool operator!=(const Rational& left, const Rational& right) {
  return !(left == right);
}

BigInteger fastPow(const BigInteger& base, const BigInteger& exp) {
  if (exp == 0) {
    return 1;
  }
  if (exp % 2 == 0) {
    return fastPow(base * base, exp / 2);
  }
  return base * fastPow(base, exp - 1);
}

std::string Rational::asDecimal(size_t precision) const {
  BigInteger tmp = abs(numerator_) * fastPow(10, precision);
  tmp /= denominator_;
  std::string answer = tmp.toString();

  while (answer.size() < precision + 1) {
    answer = '0' + answer;
  }
  answer.insert(answer.begin() + (answer.size() - precision), '.');
  if (numerator_ < 0) {
    answer = '-' + answer;
  }
  return answer;
}

Rational::Rational(const BigInteger& num, const BigInteger& denom)
    : numerator_(num), denominator_(denom) {
  if (denom.getSign()) {
    numerator_ *= -1;
    denominator_ *= -1;
  }
  greaterCommonDivisor(num, denom);
}

Rational Rational::operator-() const {
  BigInteger copy = numerator_ * -1;
  return Rational(copy, denominator_);
}

void Rational::checkSigns() {
  if (denominator_ < 0) {
    numerator_ *= -1;
    denominator_ *= -1;
  }
}

void Rational::makeIrreducible() {
  checkSigns();
  BigInteger commonDivisor =
      greaterCommonDivisor(abs(numerator_), denominator_);
  numerator_ /= commonDivisor;
  denominator_ /= commonDivisor;
}

std::string Rational::toString() {
  if (denominator_ == 1) {
    return numerator_.toString();
  }
  return numerator_.toString() + "/" + denominator_.toString();
}

BigInteger Rational::greaterCommonDivisor(const BigInteger& a,
                                          const BigInteger& b) {
  return b == 0 ? a : greaterCommonDivisor(b, a % b);
}

BigInteger Rational::getNumerator() const { return numerator_; }

BigInteger Rational::getDenominator() const { return denominator_; }

Rational::operator double() const { return std::stod(asDecimal(20)); }

Rational::operator bool() const { return numerator_ == 0; }

Rational::Rational() : numerator_(0_bi), denominator_(1_bi) {}

Rational::Rational(const BigInteger& val)
    : numerator_(val), denominator_(1_bi) {}

Rational::Rational(long long int int_val)
    : numerator_(int_val), denominator_(1_bi) {}

