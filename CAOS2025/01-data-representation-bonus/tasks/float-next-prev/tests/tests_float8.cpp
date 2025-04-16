
#include "utils.h"

TEST(test_float8_div_plain, Suite::div()) {
    // clang-format off
    test_bin_op(float_div, 3, 4,
        "0",   0, 0x0, 0x0,
        "0",   0, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_div, 3, 4,
        "0",   0, 0x0, 0x0,
        "-0",  1, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_div, 3, 4,
        "-0",  1, 0x0, 0x0,
        "0",   0, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_div, 3, 4,
        "-0",  1, 0x0, 0x0,
        "-0",  1, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_div, 3, 4,
        "1",   0, 0x3, 0x0,
        "0",   0, 0x0, 0x0,
        "Inf", 0, 0x7, 0x0);
    test_bin_op(float_div, 3, 4,
        "1",    0, 0x3, 0x0,
        "-0",   1, 0x0, 0x0,
        "-Inf", 1, 0x7, 0x0);
    test_bin_op(float_div, 3, 4,
        "-1",   1, 0x3, 0x0,
        "0",    0, 0x0, 0x0,
        "-Inf", 1, 0x7, 0x0);
    test_bin_op(float_div, 3, 4,
        "0",   0, 0x0, 0x0,
        "Inf", 0, 0x7, 0x0,
        "0",   0, 0x0, 0x0);
    test_bin_op(float_div, 3, 4,
        "0",    0, 0x0, 0x0,
        "-Inf", 1, 0x7, 0x0,
        "-0",   1, 0x0, 0x0);
    test_bin_op(float_div, 3, 4,
        "5",   0, 0x5, 0x4,
        "2",   0, 0x4, 0x0,
        "2.5", 0, 0x4, 0x4);
    test_bin_op(float_div, 3, 4,
        "5",    0, 0x5, 0x4,
        "-2",   1, 0x4, 0x0,
        "-2.5", 1, 0x4, 0x4);
    test_bin_op(float_div, 3, 4,
        "0.15625E-1", 0, 0x0, 0x1,
        "2",          0, 0x4, 0x0,
        "0",          0, 0x0, 0x0);
    // clang-format on
}

TEST(test_float8_div_nan, Suite::div()) {
    // clang-format off
    test_bin_op(float_div, 3, 4,
        "-2",  1, 0x4, 0x0,
        "NaN", 0, 0x7, 0x8,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_div, 3, 4,
        "NaN", 0, 0x7, 0x8,
        "0",   0, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_div, 3, 4,
        "NaN", 0, 0x7, 0x8,
        "NaN", 0, 0x7, 0x8,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_div, 3, 4,
        "NaN", 0, 0x7, 0x8,
        "0",   0, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_div, 3, 4,
        "0",   0, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8,
        "NaN", 0, 0x7, 0x8);
    // clang-format on
}

TEST(test_float8_add_overflow, Suite::add()) {
    // clang-format off
    test_bin_op(float_add, 3, 4,
          "1E1", 0, 0x6, 0x4,
          "1E1", 0, 0x6, 0x4,
          "Inf", 0, 0x7, 0x0);
    test_bin_op(float_add, 3, 4,
        "-1E1", 1, 0x6, 0x4,
        "-1E1", 1, 0x6, 0x4,
        "-Inf", 1, 0x7, 0x0);
    // clang-format on
}

TEST(test_float8_mul_overflow, Suite::mul()) {
    // clang-format off
    test_bin_op(float_mul, 3, 4,
        "1E1", 0, 0x6, 0x4,
        "1E1", 0, 0x6, 0x4,
        "Inf", 0, 0x7, 0x0);
    test_bin_op(float_mul, 3, 4,
        "1E1",  0, 0x6, 0x4,
        "-1E1", 1, 0x6, 0x4,
        "-Inf", 1, 0x7, 0x0);
    test_bin_op(float_mul, 3, 4,
        "-1E1", 1, 0x6, 0x4,
        "1E1",  0, 0x6, 0x4,
        "-Inf", 1, 0x7, 0x0);
    test_bin_op(float_mul, 3, 4,
        "Inf", 0, 0x7, 0x0,
        "5",   0, 0x5, 0x4,
        "Inf", 0, 0x7, 0x0);
    test_bin_op(float_mul, 3, 4,
        "5",   0, 0x5, 0x4,
        "Inf", 0, 0x7, 0x0,
        "Inf", 0, 0x7, 0x0);
    test_bin_op(float_mul, 3, 4,
        "Inf", 0, 0x7, 0x0,
        "Inf", 0, 0x7, 0x0,
        "Inf", 0, 0x7, 0x0);
    test_bin_op(float_mul, 3, 4,
        "Inf",  0, 0x7, 0x0,
        "-Inf", 1, 0x7, 0x0,
        "-Inf", 1, 0x7, 0x0);
    test_bin_op(float_mul, 3, 4,
        "-Inf", 1, 0x7, 0x0,
        "Inf",  0, 0x7, 0x0,
        "-Inf", 1, 0x7, 0x0);
    // clang-format on
}

TEST(test_float8_next, Suite::prev_next()) {
    // clang-format off
    test_unary_op(float_next, 3, 4,
        "1",      0, 0x3, 0x0,
        "1.0625", 0, 0x3, 0x1);
    test_unary_op(float_next, 3, 4,
        "15.5", 0, 0x6, 0xf,
        "Inf",  0, 0x7, 0x0);
    test_unary_op(float_next, 3, 4,
        "-0.15625E-1", 1, 0x0, 0x1,
        "-0",          1, 0x0, 0x0);
    test_unary_op(float_next, 3, 4,
        "-0", 1, 0x0, 0x0,
        "0",  0, 0x0, 0x0);
    test_unary_op(float_next, 3, 4,
        "0",          0, 0x0, 0x0,
        "0.15625E-1", 0, 0x0, 0x1);
    test_unary_op(float_next, 3, 4,
        "-Inf",  1, 0x7, 0x0,
        "-15.5", 1, 0x6, 0xf);
    test_unary_op(float_next, 3, 4,
        "Inf",  0, 0x7, 0x0,
        "Inf",  0, 0x7, 0x0);
    test_unary_op(float_next, 3, 4,
        "NaN",  0, 0x7, 0x8,
        "NaN",  0, 0x7, 0x8);
    // clang-format on
}

TEST(test_float8_prev, Suite::prev_next()) {
    // clang-format off
    test_unary_op(float_prev, 3, 4,
        "1.0625", 0, 0x3, 0x1,
        "1",      0, 0x3, 0x0);
    test_unary_op(float_prev, 3, 4,
        "Inf",  0, 0x7, 0x0,
        "15.5", 0, 0x6, 0xf);
    test_unary_op(float_prev, 3, 4,
        "-0",          1, 0x0, 0x0,
        "-0.15625E-1", 1, 0x0, 0x1);
    test_unary_op(float_prev, 3, 4,
        "0",  0, 0x0, 0x0,
        "-0", 1, 0x0, 0x0);
    test_unary_op(float_prev, 3, 4,
        "0.15625E-1", 0, 0x0, 0x1,
        "0",          0, 0x0, 0x0);
    test_unary_op(float_prev, 3, 4,
        "-15.5", 1, 0x6, 0xf,
        "-Inf",  1, 0x7, 0x0);
    test_unary_op(float_prev, 3, 4,
        "-Inf",  1, 0x7, 0x0,
        "-Inf",  1, 0x7, 0x0);
    test_unary_op(float_prev, 3, 4,
        "NaN",  0, 0x7, 0x8,
        "NaN",  0, 0x7, 0x8);
    // clang-format on
}

TEST(test_float8_mul_nan, Suite::mul()) {
    // clang-format off
    test_bin_op(float_mul, 3, 4,
        "-2",  1, 0x4, 0x0,
        "NaN", 0, 0x7, 0x8,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_mul, 3, 4,
        "NaN", 0, 0x7, 0x8,
        "0",   0, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_mul, 3, 4,
        "NaN", 0, 0x7, 0x8,
        "NaN", 0, 0x7, 0x8,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_mul, 3, 4,
        "Inf", 0, 0x7, 0x0,
        "0",   0, 0x0, 0x0,
        "NaN", 0, 0x7, 0x8);
    test_bin_op(float_mul, 3, 4,
        "0",   0, 0x0, 0x0,
        "Inf", 0, 0x7, 0x0,
        "NaN", 0, 0x7, 0x8);
    // clang-format on
}

TEST(test_float8_parse, Suite::string()) {
    // clang-format off
    test_parse(3, 4, "inf",        0, 0x7, 0x0); // Inf
    test_parse(3, 4, "Inf",        0, 0x7, 0x0); // Inf
    test_parse(3, 4, "-inf",       1, 0x7, 0x0); // Inf
    test_parse(3, 4, "nan",        0, 0x7, 0x8); // NaN
    test_parse(3, 4, "NaN",        0, 0x7, 0x8); // NaN
    test_parse(3, 4, "0",          0, 0x0, 0x0); // 0
    test_parse(3, 4, "1",          0, 0x3, 0x0); // 1
    test_parse(3, 4, "15",         0, 0x6, 0xe); // 15
    test_parse(3, 4, "15.244449",  0, 0x6, 0xe); // 15
    test_parse(3, 4, "15.25",      0, 0x6, 0xe); // 15.5
    test_parse(3, 4, "15.5",       0, 0x6, 0xf); // 15.5
    test_parse(3, 4, "15.500001",  0, 0x7, 0x0); // Inf
    test_parse(3, 4, "-15",        1, 0x6, 0xe); // -15
    test_parse(3, 4, "-15.244449", 1, 0x6, 0xe); // -15
    test_parse(3, 4, "-15.25",     1, 0x6, 0xe); // -15.5
    test_parse(3, 4, "-15.5",      1, 0x6, 0xf); // -15.5
    test_parse(3, 4, "-15.500001", 1, 0x7, 0x0); // -Inf
    // clang-format on
}

TEST(test_float8_to_string, Suite::string()) {
    // clang-format off
    test_to_string(3, 4, "Inf",    0, 0x7, 0x0); // Inf
    test_to_string(3, 4, "NaN",    0, 0x7, 0x8); // NaN
    test_to_string(3, 4, "0",      0, 0x0, 0x0); // 0
    test_to_string(3, 4, "1",      0, 0x3, 0x0); // 1
    test_to_string(3, 4, "-1",     1, 0x3, 0x0); // 1
    // clang-format on
}