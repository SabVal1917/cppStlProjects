
#include <bit>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_set>

#include "utils.h"

template <> int get_mantissa_bits<float>() { return 23; };

template <> int get_mantissa_bits<double>() { return 52; };

template <> int get_exponent_bits<float>() { return 8; };

template <> int get_exponent_bits<double>() { return 11; };

void set_float(Float* flt, int sign, uint64_t exponent, uint64_t mantissa) {
    float_set_sign(flt, sign);
    float_set_exponent(flt, &exponent);
    float_set_mantissa(flt, &mantissa);
}

template <> void set_to_native<float>(Float* flt, float real_float) {
    uint32_t flt_bytes = 0;
    memcpy(&flt_bytes, &real_float, sizeof(float));

    uint64_t sign = (flt_bytes >> 31) & ((1ull << 1) - 1);
    uint64_t exponent = (flt_bytes >> 23) & ((1ull << 8) - 1);
    uint64_t mantissa = (flt_bytes >> 0) & ((1ull << 23) - 1);

    set_float(flt, sign, exponent, mantissa);
}

template <> void set_to_native<double>(Float* flt, double real_double) {
    uint64_t flt_bytes = 0;
    memcpy(&flt_bytes, &real_double, sizeof(double));

    uint64_t sign = (flt_bytes >> 63) & ((1ull << 1) - 1);
    uint64_t exponent = (flt_bytes >> 52) & ((1ull << 11) - 1);
    uint64_t mantissa = (flt_bytes >> 0) & ((1ull << 52) - 1);

    set_float(flt, sign, exponent, mantissa);
}

template <> void as_native_float<float>(Float* flt, float& result) {
    uint32_t sign = float_get_sign(flt), mantissa = 0;
    uint8_t exponent = 0;

    float_get_exponent(flt, &exponent);
    float_get_mantissa(flt, &mantissa);

    uint32_t float_bytes = (sign << 31) | ((uint32_t)exponent << 23) | mantissa;
    memcpy(&result, &float_bytes, sizeof(float));
}

template <> void as_native_float<double>(Float* flt, double& result) {
    uint64_t mantissa = 0;
    uint16_t exponent = 0;
    bool sign = float_get_sign(flt);

    // Use 7-byte buffer here to trigger asan warning on buffer-overflow
    char mantissa_buffer[7] = {};

    float_get_exponent(flt, &exponent);
    float_get_mantissa(flt, mantissa_buffer);

    memcpy(&mantissa, mantissa_buffer, sizeof(mantissa_buffer));

    uint64_t float_bytes = ((uint64_t)sign << 63) | ((uint64_t)exponent << 52) | mantissa;
    memcpy(&result, &float_bytes, sizeof(double));
}

bool cmp_float(Float* a, Float* b) {
    // We'll not exceed 1024 bits in both mantissa and exponent
    uint64_t mantissa_a[16]{}, mantissa_b[16]{};
    uint64_t exponent_a[16]{}, exponent_b[16]{};

    int sign_a = float_get_sign(a);
    int sign_b = float_get_sign(b);
    float_get_exponent(a, exponent_a);
    float_get_mantissa(a, mantissa_a);
    float_get_exponent(b, exponent_b);
    float_get_mantissa(b, mantissa_b);

    return sign_a == sign_b && memcmp(mantissa_a, mantissa_b, sizeof(mantissa_a)) == 0 &&
           memcmp(exponent_a, exponent_a, sizeof(exponent_a)) == 0;
}

void test_bin_op(FloatBinOp* op, int e, int m, const char* s1, int sign1, uint64_t e1, uint64_t m1, const char* s2,
                 int sign2, uint64_t e2, uint64_t m2, const char* s3, int sign3, uint64_t e3, uint64_t m3,
                 nostd::source_location location) {
    TestLocation suite_location(location);

    Float a = {}, b = {}, c = {}, d = {};
    if (float_init(&a, e, m) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&b, e, m) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&c, e, m) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&d, e, m) < 0)
        HANDLE_FAIL(float_init);

    set_float(&a, sign1, e1, m1);
    set_float(&b, sign2, e2, m2);
    set_float(&d, sign3, e3, m3);
    (*op)(&c, &a, &b);

    const char* op_str = " ? ";

    if (op == float_add)
        op_str = " + ";
    else if (op == float_sub)
        op_str = " - ";
    else if (op == float_mul)
        op_str = " * ";
    else if (op == float_div)
        op_str = " / ";

    EXPECT(cmp_float(&c, &d)) << s1 << op_str << s2 << " != " << s3;

    float_destroy(&a);
    float_destroy(&b);
    float_destroy(&c);
    float_destroy(&d);
}

void test_unary_op(FloatUnaryOp* op, int e, int m, const char* s1, int sign1, uint64_t e1, uint64_t m1, const char* s2,
                   int sign2, uint64_t e2, uint64_t m2, nostd::source_location location) {
    TestLocation suite_location(location);

    Float a = {}, b = {};
    if (float_init(&a, e, m) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&b, e, m) < 0)
        HANDLE_FAIL(float_init);

    set_float(&a, sign1, e1, m1);
    set_float(&b, sign2, e2, m2);
    (*op)(&a);

    const char* op_str = " ? ";

    if (op == float_next)
        op_str = "float_next";
    else if (op == float_prev)
        op_str = "float_prev";

    EXPECT(cmp_float(&a, &b)) << op_str << "(" << s1 << ")" << " != " << s2;

    float_destroy(&a);
    float_destroy(&b);
}

bool test_parse(int e, int m, const char* str, int sign, uint64_t exponent, uint64_t mantissa,
                nostd::source_location location) {
    TestLocation suite_location(location);

    Float flt = {};
    if (float_init(&flt, e, m) < 0)
        HANDLE_FAIL_RET(float_init, true);
    float_parse(&flt, str);

    uint64_t actual_mantissa = 0, actual_exponent = 0;

    int actual_sign = float_get_sign(&flt);
    float_get_exponent(&flt, &actual_exponent);
    float_get_mantissa(&flt, &actual_mantissa);
    float_destroy(&flt);

    if (actual_sign != sign || actual_exponent != exponent || actual_mantissa != mantissa) {
        auto stream = test_fail();

        int exponent_digits = (e + 3) / 4;
        int mantissa_digits = (m + 3) / 4;

        stream << "float_parse fails for '" << str << "':\n";
        stream << "Exponent: | Correct:  ";
        stream << "0x" << std::setfill('0') << std::setw(exponent_digits) << std::hex << exponent << "\n";
        stream << "          | Your:     ";
        stream << "0x" << std::setfill('0') << std::setw(exponent_digits) << std::hex << actual_exponent << "\n";

        stream << "Mantissa: | Correct:  ";
        stream << "0x" << std::setfill('0') << std::setw(mantissa_digits) << std::hex << mantissa << "\n";
        stream << "          | Your:     ";
        stream << "0x" << std::setfill('0') << std::setw(mantissa_digits) << std::hex << actual_mantissa << "\n";

        stream << "Sign:     | Correct:  " << sign << "\n";
        stream << "          | Your:     " << actual_sign << "\n";

        return true;
    }

    return false;
}

void test_to_string(int e, int m, const char* expected_str, int sign, uint64_t exponent, uint64_t mantissa,
                    nostd::source_location location) {
    TestLocation suite_location(location);

    Float flt = {};
    if (float_init(&flt, e, m) < 0)
        HANDLE_FAIL(float_init);
    float_set_sign(&flt, sign);
    float_set_exponent(&flt, &exponent);
    float_set_mantissa(&flt, &mantissa);

    int length = float_string(&flt, nullptr, 0);
    std::string str(length, '\0');
    float_string(&flt, str.data(), str.size());

    if (str != expected_str) {
        test_fail() << "Float '" << expected_str << "' is printed as '" << str << "'.\n";
    }

    float_destroy(&flt);
}

template <> void generate_random_native_float<float>(float* native_float, Float* custom_float) {
    uint32_t random_num = rand();
    memcpy(native_float, &random_num, sizeof(float));
    set_to_native(custom_float, *native_float);
}

template <> void generate_random_native_float<double>(double* native_double, Float* custom_float) {
    uint64_t random_num = rand() | ((uint64_t)rand() << 32);
    memcpy(native_double, &random_num, sizeof(double));
    set_to_native(custom_float, *native_double);
}

ErrStream test_fail(const char* condition, nostd::source_location location) {
    current_test->failed();
    nostd::source_location* p_location = &location;
    if (current_test && current_test->location) {
        p_location = &(*current_test->location);
    }
    return ErrStream(*p_location, condition);
}

Test* current_test = NULL;

Suite* Suite::system_audit() {
    static auto suite = Suite::add_suite(0, "system_audit", "System Audit", 0);
    return suite;
}
Suite* Suite::construct() {
    static auto suite = Suite::add_suite(1, "construct", "Construct", 0);
    return suite;
}
Suite* Suite::mul() {
    static auto suite = Suite::add_suite(2, "mul", "Multiply", 100);
    return suite;
}
Suite* Suite::prev_next() {
    static auto suite = Suite::add_suite(3, "prev_next", "Previous / Next", 100);
    return suite;
}
Suite* Suite::add() {
    static auto suite = Suite::add_suite(4, "add", "Addition", 150);
    return suite;
}
Suite* Suite::div() {
    static auto suite = Suite::add_suite(5, "div", "Division", 150);
    return suite;
}
Suite* Suite::string() {
    static auto suite = Suite::add_suite(6, "string", "Parsing / Serializing", 250);
    return suite;
}

TEST(system_audit, Suite::system_audit()) {
    if (EXPECT(std::endian::native == std::endian::little) << "Your system does not use little-endian!")
        return;

    // Make sure that built-in floats use IEEE-754
    // and mantissa and exponent sizes are as expected.

    if (EXPECT(sizeof(float) == 4) << "Native float is not 4 bytes long")
        return;
    if (EXPECT(sizeof(double) == 8) << "Native double is not 4 bytes long")
        return;

    float float_one = 1.0;
    uint32_t float_one_bytes = 0;
    memcpy(&float_one_bytes, &float_one, sizeof(float));
    if (EXPECT(float_one_bytes == 0x3f800000) << "Native float does not seem to use IEEE-754 standard")
        return;

    double double_one = 1.0;
    uint64_t double_one_bytes = 0;
    memcpy(&double_one_bytes, &double_one, sizeof(double));
    if (EXPECT(double_one_bytes == 0x3ff0000000000000) << "Native double does not seem to use IEEE-754 standard")
        return;

    if (EXPECT(1.23793996e27f + 3.6893493e19f == 1.2379401e27f))
        return;
}

bool run_suite(Suite& suite) {
    bool fail = false;
    for (auto test : suite.tests) {
        if ((*test)())
            fail = true;
    }
    return fail;
}

bool run_suite_as_subprocess(Suite& suite) {
    std::cout << std::flush;
    int pid = fork();
    if (pid == 0) {
        exit(run_suite(suite) ? EXIT_FAILURE : EXIT_SUCCESS);
    } else {
        int retcode = 0;
        while (true) {
            int res = waitpid(pid, &retcode, 0);
            if (res < 0) {
                if (errno == ECHILD) {
                    retcode = -1;
                    break;
                }
                perror("wait4");
                return 1;
            }
            if (WIFSIGNALED(retcode)) {
                retcode = -1;
                break;
            }
            if (WIFEXITED(retcode)) {
                retcode = WEXITSTATUS(retcode);
                break;
            }
        }
        return retcode != 0;
    }
}

int main(int argc, const char** argv) {

    std::unordered_set<std::string> suites_to_run;

    std::sort(Suite::get_suites().begin(), Suite::get_suites().end(),
              [](auto& a, auto& b) { return a->priority < b->priority; });

    if (argc == 1) {
        for (auto& suite : Suite::get_suites()) {
            suites_to_run.insert(suite->id);
        }
    } else {
        for (int i = 1; i < argc; i++) {
            suites_to_run.insert(argv[i]);
        }
    }

    dup2(STDOUT_FILENO, STDERR_FILENO);

    int total_bonus = 0;
    bool is_failed = false;

    for (auto& suite : Suite::get_suites()) {
        std::cout << Color::Modifier(Color::FG_BLUE) << "=== " << suite->name << " (" << suite->reward << " points)"
                  << " ===" << Color::Modifier(Color::FG_DEFAULT);

        if (suites_to_run.find(suite->id) == suites_to_run.end()) {
            std::cout << " - Skipped\n\n";
            continue;
        }

        std::cout << "\n";

        bool fail = suites_to_run.size() == 1 ? run_suite(*suite) : run_suite_as_subprocess(*suite);

        if (!fail) {
            total_bonus += suite->reward;
            std::cout << Color::Modifier(Color::FG_BLUE) << "=== Suite succeded! ===";

            if (suite->reward > 0) {
                std::cout << Color::Modifier(Color::FG_DEFAULT) << " (" << Color::Modifier(Color::FG_GREEN) << "+"
                          << suite->reward << " points" << Color::Modifier(Color::FG_DEFAULT) << ")";
            }

            std::cout << Color::Modifier(Color::FG_DEFAULT) << "\n\n";
        } else {
            is_failed = true;
            std::cout << Color::Modifier(Color::FG_RED) << "=== Suite failed" << Color::Modifier(Color::FG_DEFAULT)
                      << "\n\n";
            break;
        }
    }

    std::cout << "Your total score: " << Color::Modifier(Color::FG_GREEN) << total_bonus << " points"
              << Color::Modifier(Color::FG_DEFAULT) << "\n\n";
    return is_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
