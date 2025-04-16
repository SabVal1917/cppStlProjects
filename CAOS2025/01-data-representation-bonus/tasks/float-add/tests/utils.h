#pragma once

#include "floats.h"
#include "source_location.hpp"
#include <cassert>
#include <cmath>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

struct Dummy {};

struct Suite;
struct Test;
extern Test* current_test;

template <typename T> int get_mantissa_bits();
template <> int get_mantissa_bits<float>();
template <> int get_mantissa_bits<double>();

template <typename T> int get_exponent_bits();
template <> int get_exponent_bits<float>();
template <> int get_exponent_bits<double>();

// https://stackoverflow.com/a/17469726

namespace Color {
enum Code {
    FG_RED = 31,
    FG_GREEN = 32,
    FG_BLUE = 34,
    FG_DEFAULT = 39,
    BG_RED = 41,
    BG_GREEN = 42,
    BG_BLUE = 44,
    BG_DEFAULT = 49
};
class Modifier {
    Code code;

  public:
    Modifier(Code pCode) : code(pCode) {}
    friend std::ostream& operator<<(std::ostream& os, const Modifier& mod) { return os << "\033[" << mod.code << "m"; }
};
} // namespace Color

struct Suite {
    constexpr Suite(int priority, const char* id, const char* name, int reward)
        : priority(priority), id(id), name(name), reward(reward) {}

    static Suite* add_suite(int priority, const char* id, const char* name, int reward) {
        auto suite = std::make_unique<Suite>(priority, id, name, reward);
        auto ptr = suite.get();
        get_suites().push_back(std::move(suite));
        return ptr;
    }

    static std::vector<std::unique_ptr<Suite>>& get_suites() {
        static std::vector<std::unique_ptr<Suite>> suites;
        return suites;
    }

    void add_test(std::function<bool()>* test) { tests.push_back(test); }

    int priority = 0;
    const char* id = NULL;
    const char* name = NULL;
    int reward = 0;

    static Suite* system_audit();
    static Suite* construct();
    static Suite* mul();
    static Suite* prev_next();
    static Suite* add();
    static Suite* div();
    static Suite* string();

    std::vector<std::function<bool()>*> tests;
};

struct Test {
    Test(const char* name) : name(name) {
        assert(current_test == nullptr);
        print_header();
        current_test = this;
        if (getenv("CI") != nullptr) {
            silent = true;
        }
    }
    void set_source_location(const std::optional<nostd::source_location> location = nostd::source_location::current()) {
        Test::location = location;
    }

    bool has_source_location() { return location.has_value(); }

    void failed() {
        if (!finished) {
            finished = true;
            successful = false;
            clear_line();
            print_header();
            std::cout << Color::Modifier(Color::FG_RED) << "FAIL" << Color::Modifier(Color::FG_DEFAULT) << "\n";
        }
    }

    ~Test() {
        finished = true;
        if (successful) {
            clear_line();
            print_header();
            std::cout << Color::Modifier(Color::FG_GREEN) << "PASS" << Color::Modifier(Color::FG_DEFAULT) << "\n";
        }
        current_test = nullptr;
    }

    void set_progress(int progress, int total) {
        Test::progress = progress;
        Test::total = total;
        if (!finished && successful && !silent) {
            clear_line();
            print_header();
        }
    }

    void clear_line() const { printf("\33[2K\r"); }

    void print_header() const {
        //        if(finished) {
        //            if(successful) {
        //                std::cout << Color::Modifier(Color::FG_GREEN);
        //            } else {
        //                std::cout << Color::Modifier(Color::FG_RED);
        //            }
        //        }
        std::cout << "[" << name;
        if (total != 0 && !finished) {
            std::cout << " (" << progress << " / " << total << ")";
        }
        std::cout << "]: ";
        //        if(successful) {
        //            std::cout << Color::Modifier(Color::FG_DEFAULT);
        //        }
        std::flush(std::cout);
    }

    static Dummy add_test(std::function<bool()> test, Suite* suite) {
        auto test_ptr = std::make_unique<std::function<bool()>>(test);
        suite->add_test(test_ptr.get());
        get_tests().push_back(std::move(test_ptr));
        return {};
    }

    static std::vector<std::unique_ptr<std::function<bool()>>>& get_tests() {
        static std::vector<std::unique_ptr<std::function<bool()>>> tests;
        return tests;
    }

    std::optional<nostd::source_location> location;
    const char* name;
    bool successful = true;
    bool finished = false;
    bool silent = false;
    int progress = 0;
    int total = 0;
};

struct TestLocation {
    TestLocation(const nostd::source_location location = nostd::source_location::current()) {
        if (current_test->has_source_location())
            inactive = true;
        else
            current_test->set_source_location(location);
    }
    ~TestLocation() {
        if (!inactive)
            current_test->set_source_location(std::nullopt);
    }
    bool inactive = false;
};

struct ErrStream {
    ErrStream() : disabled(true) {}
    ErrStream(nostd::source_location& loc, const char* message) : default_message(message) {
        sstream << loc.file_name() << ":" << loc.line() << ": ";
    }
    ~ErrStream() {
        if (disabled)
            return;
        if (default_message != nullptr) {
            sstream << default_message;
        }

        std::string line;
        std::cout << Color::Modifier(Color::FG_RED);
        while (std::getline(sstream, line)) {
            std::cout << "[" << current_test->name;
            if (current_test->total != 0) {
                std::cout << " (" << current_test->progress << " / " << current_test->total << ")";
            }
            std::cout << "]: " << line << "\n";
        }
        std::cout << Color::Modifier(Color::FG_DEFAULT);
        std::flush(std::cout);
    }

    operator bool() const { return !disabled; }

    mutable std::stringstream sstream;
    mutable bool disabled = false;
    mutable const char* default_message = nullptr;
};

template <typename T> const ErrStream& operator<<(const ErrStream& stream, const T& obj) {
    if (stream) {
        stream.sstream << obj;
        stream.default_message = nullptr;
    }
    return stream;
}

ErrStream test_fail(const char* condition = nullptr,
                    nostd::source_location location = nostd::source_location::current());

#define HANDLE_FAIL_RET(name, ret)                                                                                     \
    {                                                                                                                  \
        test_fail() << #name " returned failure code";                                                                 \
        return ret;                                                                                                    \
    }
#define HANDLE_FAIL(name) HANDLE_FAIL_RET(name, )
#define EXPECT(condition) (condition ? ErrStream() : test_fail(#condition))
#define TEST(name, suite)                                                                                              \
    void inner_##name##_body();                                                                                        \
    bool name() {                                                                                                      \
        Test test(#name);                                                                                              \
        inner_##name##_body();                                                                                         \
        return !test.successful;                                                                                       \
    }                                                                                                                  \
    auto inner_##name##_registry = Test::add_test(name, suite);                                                        \
    void inner_##name##_body()

typedef void FloatBinOp(Float*, const Float*, const Float*);
typedef void FloatUnaryOp(Float*);

void set_float(Float* flt, int sign, uint64_t exponent, uint64_t mantissa);
void set_float32(Float* flt, float real_flt);
void set_float64(Float* flt, double real_flt);
bool cmp_float(Float* a, Float* b);

void test_bin_op(FloatBinOp* op, int e, int m, const char* s1, int sign1, uint64_t e1, uint64_t m1, const char* s2,
                 int sign2, uint64_t e2, uint64_t m2, const char* s3, int sign3, uint64_t e3, uint64_t m3,
                 nostd::source_location location = nostd::source_location::current());

void test_unary_op(FloatUnaryOp* op, int e, int m, const char* s1, int sign1, uint64_t e1, uint64_t m1, const char* s2,
                   int sign2, uint64_t e2, uint64_t m2,
                   nostd::source_location location = nostd::source_location::current());

bool test_parse(int e, int m, const char* str, int sign, uint64_t exponent, uint64_t mantissa,
                nostd::source_location location = nostd::source_location::current());

void test_to_string(int e, int m, const char* expected_str, int sign, uint64_t exponent, uint64_t mantissa,
                    nostd::source_location location = nostd::source_location::current());

template <typename T>
bool compare_with_std_to_string(int sign, uint64_t exponent, uint64_t mantissa,
                                nostd::source_location location = nostd::source_location::current()) {
    TestLocation suite_location(location);

    Float flt = {}, parsed_flt = {};
    if (float_init(&flt, get_exponent_bits<T>(), get_mantissa_bits<T>()) < 0)
        HANDLE_FAIL_RET(float_init, true);
    if (float_init(&parsed_flt, get_exponent_bits<T>(), get_mantissa_bits<T>()) < 0)
        HANDLE_FAIL_RET(float_init, true);

    float_set_sign(&flt, sign);
    float_set_exponent(&flt, &exponent);
    float_set_mantissa(&flt, &mantissa);

    T native_float = 0;
    as_native_float(&flt, native_float);

    int length = float_string(&flt, nullptr, 0);
    std::string str(length, '\0');
    float_string(&flt, str.data(), str.size());

    T parsed_native_float;

    if constexpr (std::is_same_v<T, float>) {
        parsed_native_float = strtof(str.data(), NULL);
    } else if constexpr (std::is_same_v<T, double>) {
        parsed_native_float = strtod(str.data(), NULL);
    }

    set_to_native(&parsed_flt, parsed_native_float);
    float_destroy(&parsed_flt);
    float_destroy(&flt);

    if (memcmp(&native_float, &parsed_native_float, sizeof(T)) != 0) {
        auto stream = test_fail();
        stream << "float_to_string fails for '" << native_float << "': real output is '" << parsed_native_float << "'";
        maybe_print_rounding_hint(stream, native_float, parsed_native_float);
        return true;
    }

    return false;
}

template <typename T> bool fuzz_to_string() {
    Float a = {};
    T real_a = 0;

    float_init(&a, get_exponent_bits<T>(), get_mantissa_bits<T>());

    for (int i = 0; i < 1000; i++) {
        current_test->set_progress(i, 1000);
        bool random_sign = rand() & (1 << 1);
        uint64_t random_exponent = rand() % ((1ll << get_exponent_bits<T>()) - 1); // Not generating NaNs and infs
        uint64_t random_mantissa = rand() % (1ll << get_mantissa_bits<T>());

        if (compare_with_std_to_string<T>(random_sign, random_exponent, random_mantissa)) {
            return true;
        }
    }

    float_destroy(&a);
    return false;
}

template <typename T> bool fuzz_parse() {
    Float a = {};
    T real_a = 0;

    float_init(&a, get_exponent_bits<T>(), get_mantissa_bits<T>());

    for (int i = 0; i < 1000; i++) {
        current_test->set_progress(i, 1000);
        bool random_sign = rand() & (1 << 1);
        uint64_t random_exponent = rand() % ((1ll << get_exponent_bits<T>()) - 1); // Not generating NaNs and infs
        uint64_t random_mantissa = rand() % (1ll << get_mantissa_bits<T>());

        float_set_sign(&a, random_sign);
        float_set_exponent(&a, &random_exponent);
        float_set_mantissa(&a, &random_mantissa);

        T native_float = 0;
        as_native_float(&a, native_float);

        // Not using std::to_string() here since it would produce 0.000000 for
        // small non-zero floats
        std::stringstream stream;
        stream << std::setprecision(get_mantissa_bits<T>() / 3 + 2) << native_float;
        auto native_string = stream.str();

        if (test_parse(get_exponent_bits<T>(), get_mantissa_bits<T>(), native_string.data(), random_sign,
                       random_exponent, random_mantissa)) {
            float_destroy(&a);
            return true;
        }
    }

    float_destroy(&a);
    return false;
}

template <typename T> void generate_random_native_float(T* native_float, Float* custom_float);

template <typename T> bool is_quiet_nan(T input) {
    uint64_t bytes = 0;
    memcpy(&bytes, &input, sizeof(T));

    int mantissa_length = get_mantissa_bits<T>();
    return (bytes >> mantissa_length) & 1;
}

template <typename T> uint64_t get_nan_payload(T input) {
    uint64_t bytes = 0;
    memcpy(&bytes, &input, sizeof(T));

    int mantissa_length = get_mantissa_bits<T>();
    int payload_length = mantissa_length - 1;
    return (bytes & ((1ll << payload_length) - 1));
}

template <typename T> void print_nan(ErrStream& stream, T nan) {
    int mantissa_length = get_mantissa_bits<T>();
    int payload_length = mantissa_length - 1;

    assert(mantissa_length != 0);

    uint64_t bytes = 0;
    memcpy(&bytes, &nan, sizeof(T));
    stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << bytes << " (";

    if (is_quiet_nan(nan))
        stream << "quiet nan,     ";
    else
        stream << "signaling nan, ";

    stream << "payload = 0x" << std::setfill('0') << std::setw((payload_length + 3) / 4) << std::hex
           << get_nan_payload(nan) << ")\n";
}

template <typename T> void maybe_print_rounding_hint(ErrStream& stream, T expected, T actual) {
    if (std::nextafter(expected, std::numeric_limits<T>::infinity()) == actual ||
        std::nextafter(actual, std::numeric_limits<T>::infinity()) == expected) {
        stream << "\n";
        stream << "=== [HINT] ===\n";
        stream << "  You seem to have a rounding issue. These floats are "
                  "adjacent.\n";
        stream << "\n";
        stream << "  You are supposed to implement 'to nearest, ties to even' "
                  "rounding:\n";
        stream << "  - By default, round to the nearest value;\n";
        stream << "  - If the number falls midway, round to the nearest value "
                  "with an even least significant digit.\n";
        stream << "\n";
        stream << "  See: https://en.wikipedia.org/wiki/IEEE_754#Rounding_rules\n";
        stream << "===\n";
    }
}

template <typename T>
bool cmp_native_floats(T a, const char* op, T b, T expected, T actual,
                       nostd::source_location location = nostd::source_location::current()) {
    TestLocation suite_location(location);

    if (memcmp(&expected, &actual, sizeof(T)) != 0) {
        if (std::isnan(expected) && std::isnan(actual)) {
            // According to Wikipedia, it's fine to use both nan inputs payload
            // in the result.
            if (is_quiet_nan(expected) == is_quiet_nan(actual) &&
                (get_nan_payload(expected) == get_nan_payload(a) || get_nan_payload(expected) == get_nan_payload(b))) {
                return false;
            }
        }
        auto stream = test_fail();
        stream << std::setprecision(15);
        stream << a << " " << op << " " << b << " = " << actual << " (expected " << expected << ")\n";

        if (std::isnan(expected) && std::isnan(actual)) {
            stream << "\n";
            stream << "=== [NOTE] ===\n";
            stream << "  These nans are different:\n";
            if (std::isnan(a))
                stream << "   - lhs nan in hex:     ";
            print_nan(stream, a);
            if (std::isnan(b))
                stream << "   - rhs nan in hex:     ";
            print_nan(stream, b);
            stream << "   - Your nan in hex:    ";
            print_nan(stream, actual);
            stream << "   - Correct nan in hex: ";
            print_nan(stream, expected);
            stream << "\n";
            stream << "=== [HINT] ===:\n";
            stream << "  The key elements of proper nan handling are:\n";
            stream << "  - On any operation, the nan payload of any input nan "
                      "operand is preserved;\n";
            stream << "  - On any operation, any signaling nan is converted to "
                      "quiet nan by adding the quiet bit.\n";
            stream << "\n";
            stream << "  These rules are described well on the Wikipedia:\n";
            stream << "  - https://en.wikipedia.org/wiki/NaN#Quiet_NaN\n";
            stream << "  - https://en.wikipedia.org/wiki/NaN#Signaling_NaN\n";
            stream << "  - https://en.wikipedia.org/wiki/NaN#Encoding (about "
                      "payload transfer)\n";
            stream << "\n";
            stream << "===\n";
        } else {
            maybe_print_rounding_hint(stream, expected, actual);
        }

        return true;
    }

    return false;
}

template <typename T> void as_native_float(Float* flt, T& result);

template <typename T> void set_to_native(Float* flt, T real_double);

template <typename T>
void fuzz_bin_op(int exponent, int mantissa, const char* op_name, FloatBinOp* op, std::function<T(T, T)> f,
    int iterations = 10000, nostd::source_location location = nostd::source_location::current()) {
    TestLocation suite_location(location);
    srand(42);

    Float a = {}, b = {}, result = {};
    T real_a = 0, real_b = 0, expected_result = 0;
    const char* ops[] = {"+", "-", "*", "/"};

    if (float_init(&a, exponent, mantissa) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&b, exponent, mantissa) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&result, exponent, mantissa) < 0)
        HANDLE_FAIL(float_init);

    for (int i = 0; i < iterations; i++) {
        current_test->set_progress(i, iterations);
        generate_random_native_float(&real_a, &a);
        generate_random_native_float(&real_b, &b);

        T real_result = 0;

        op(&result, &a, &b);
        expected_result = f(real_a, real_b);

        as_native_float(&result, real_result);

        if (cmp_native_floats(real_a, op_name, real_b, expected_result, real_result)) {
            return;
        }
    }

    float_destroy(&a);
    float_destroy(&b);
    float_destroy(&result);
}
