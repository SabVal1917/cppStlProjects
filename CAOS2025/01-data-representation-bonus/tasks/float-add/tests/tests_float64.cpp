
#include "utils.h"

TEST(test_float64_fuzz_add, Suite::add()) {
    fuzz_bin_op<double>(11, 52, "+", float_add, [](auto a, auto b) { return a + b; });
}

TEST(test_float64_fuzz_sub, Suite::add()) {
    fuzz_bin_op<double>(11, 52, "-", float_sub, [](auto a, auto b) { return a - b; });
}

TEST(test_float64_fuzz_mul, Suite::mul()) {
    // 100000 iterations here is to ban solutions with DynNum
    fuzz_bin_op<double>(11, 52, "*", float_mul, [](auto a, auto b) { return a * b; }, 100000);
}

TEST(test_float64_fuzz_div, Suite::div()) {
    fuzz_bin_op<double>(11, 52, "/", float_div, [](auto a, auto b) { return a / b; });
}

TEST(test_float64_parse, Suite::string()) {
    // clang-format off
    test_parse(11, 52, "-1",     1, 0x3ff, 0x0000000000000);
    test_parse(11, 52, "1",      0, 0x3ff, 0x0000000000000);
    test_parse(11, 52, "0.9999", 0, 0x3fe, 0xfff2e48e8a71e);
    // clang-format on
}

TEST(test_float64_to_string, Suite::string()) {
    // clang-format off
    test_to_string(11, 52, "-1",     1, 0x3ff, 0x0000000000000);
    test_to_string(11, 52, "1",      0, 0x3ff, 0x0000000000000);
    test_to_string(11, 52, "0.5",    0, 0x3fe, 0x0000000000000);
    // clang-format on

    compare_with_std_to_string<double>(0, 0x400, 0x921fb54442d18); // 3.14159265358979311599796346854
    compare_with_std_to_string<double>(0, 0x000, 0x0000000000001);
    compare_with_std_to_string<double>(0, 0x001, 0x0000000000000);
}

TEST(test_float64_to_string_fuzz, Suite::string()) { fuzz_to_string<double>(); }

TEST(test_float64_parse_fuzz, Suite::string()) { fuzz_parse<double>(); }