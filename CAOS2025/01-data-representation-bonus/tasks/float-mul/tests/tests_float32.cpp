
#include "utils.h"

TEST(test_float32_add_plain, Suite::add()) {
    // clang-format off
    test_bin_op(float_add, 8, 23,
        "1", 0, 0x7f, 0x000000,
        "1", 0, 0x7f, 0x000000,
        "2", 0, 0x80, 0x000000);
    test_bin_op(float_add, 8, 23,
        "0", 0, 0x00, 0x000000,
        "1", 0, 0x7f, 0x000000,
        "1", 0, 0x7f, 0x000000);
    test_bin_op(float_add, 8, 23,
        "1", 0, 0x7f, 0x000000,
        "0", 0, 0x00, 0x000000,
        "1", 0, 0x7f, 0x000000);
    test_bin_op(float_add, 8, 23,
        "0", 0, 0x00, 0x000000,
        "1", 0, 0x7f, 0x000000,
        "1", 0, 0x7f, 0x000000);
    test_bin_op(float_add, 8, 23,
        "1", 0, 0x7f, 0x000000,
        "0", 0, 0x00, 0x000000,
        "1", 0, 0x7f, 0x000000);
    test_bin_op(float_add, 8, 23,
        "1", 0, 0x7f, 0x000000,
        "2", 0, 0x80, 0x000000,
        "3", 0, 0x80, 0x400000);
    test_bin_op(float_add, 8, 23,
        "2", 0, 0x80, 0x000000,
        "1", 0, 0x7f, 0x000000,
        "3", 0, 0x80, 0x400000);
    test_bin_op(float_add, 8, 23,
        "2", 0, 0x80, 0x000000,
        "2", 0, 0x80, 0x000000,
        "4", 0, 0x81, 0x000000);
    test_bin_op(float_add, 8, 23,
        "2", 0, 0x80, 0x000000,
        "4", 0, 0x81, 0x000000,
        "6", 0, 0x81, 0x400000);
    test_bin_op(float_add, 8, 23,
        "4", 0, 0x81, 0x000000,
        "2", 0, 0x80, 0x000000,
        "6", 0, 0x81, 0x400000);
    test_bin_op(float_add, 8, 23,
        "4", 0, 0x81, 0x000000,
        "4", 0, 0x81, 0x000000,
        "8", 0, 0x82, 0x000000);
    test_bin_op(float_add, 8, 23,
        "4",  0, 0x81, 0x000000,
        "-2", 1, 0x80, 0x000000,
        "2",  0, 0x80, 0x000000);
    test_bin_op(float_add, 8, 23,
        "-2", 1, 0x80, 0x000000,
        "4",  0, 0x81, 0x000000,
        "2",  0, 0x80, 0x000000);
    test_bin_op(float_add, 8, 23,
        "4",  0, 0x81, 0x000000,
        "-4", 1, 0x81, 0x000000,
        "0",  0, 0x00, 0x000000);
    test_bin_op(float_add, 8, 23,
        "4",  0, 0x81, 0x000000,
        "-6", 1, 0x81, 0x400000,
        "-2", 1, 0x80, 0x000000);
    test_bin_op(float_add, 8, 23,
        "-2", 1, 0x80, 0x000000,
        "6",  0, 0x81, 0x400000,
        "4",  0, 0x81, 0x000000);

    test_bin_op(float_add, 8, 23,
        "-89.0703735351562", 1, 0x85, 0x322408,
        "504.283477783203",  0, 0x87, 0x7c2449,
        "415.213104248046",  0, 0x87, 0x4f9b47);
    test_bin_op(float_add, 8, 23,
        "-1976786176", 1, 0x9d, 0x6ba6ba,
        "-5480851456", 1, 0x9f, 0x235794,
        "-7457637376", 1, 0x9f, 0x5e4142);
    test_bin_op(float_add, 8, 23,
        "307324176667294E12", 0, 0xd6, 0x7e366d,
        "-133792643060806E5", 1, 0xbe, 0x39acb2,
        "307324158220550E12", 0, 0xd6, 0x7e366c);
    test_bin_op(float_add, 8, 23,
        "781338141943398E1",    0, 0xb3, 0x5e11d5,
        "0.149820756632834E-3", 0, 0x72, 0x1d1934,
        "781338141943398E1",    0, 0xb3, 0x5e11d5);
    test_bin_op(float_add, 8, 23,
        "-0.564394667890155E-6", 1, 0x6a, 0x1780e8,
        "-1711645130752E1",      1, 0xaa, 0x7913c0,
        "-1711645130752E1",      1, 0xaa, 0x7913c0);
    test_bin_op(float_add, 8, 23,
        "-519.782653808593", 1, 0x88, 0x01f217,
        "-1123.38879394531", 1, 0x89, 0x0c6c71,
        "-1643.17138671875", 1, 0x89, 0x4d657c);
    test_bin_op(float_add, 8, 23,
        "-608562275339331E8", 1, 0xca, 0x4e305d,
        "-166224450769527E8", 1, 0xc8, 0x6146b1,
        "-774786759885856E8", 1, 0xcb, 0x034105);
    test_bin_op(float_add, 8, 23,
        "-379979818822828E7", 1, 0xc6, 0x4dfccb,
        "-396739151472361E3", 1, 0xb9, 0x303005,
        "-380019506794545E7", 1, 0xc6, 0x4e024d);
    test_bin_op(float_add, 8, 23,
        "-287482.875",    1, 0x91, 0x0c5f5c,
        "43678.45703125", 0, 0x8e, 0x2a9e75,
        "-243804.421875", 1, 0x90, 0x6e171b);
    test_bin_op(float_add, 8, 23,
        "-0.232348429564650E-37", 1, 0x01, 0x7d014a,
        "0.303570937416300E-37",  0, 0x02, 0x25479d,
        "0.712225078516502E-38",  0, 0x00, 0x4d8df0);
    test_bin_op(float_add, 8, 23,
        "3856.93188476562",  0, 0x8a, 0x710ee9,
        "-73.0775146484375", 1, 0x85, 0x1227b0,
        "3783.8544921875",   0, 0x8a, 0x6c7dac);

    test_bin_op(float_add, 8, 23,
        "1.23793996e27", 0, 0xd8, 0x7fffff,
        "3.6893493e19",  0, 0xc0, 0x000001,
        "1.2379401e27",  0, 0xd9, 0x000000);
    // clang-format on
}

TEST(test_float32_div, Suite::div()) {
    // clang-format off
    test_bin_op(float_div, 8, 23,
        "0.638458515693744E-19", 0, 0x3f, 0x16c073,
        "833818390059557E6",     0, 0xc4, 0x34ce38,
        "0.765711519861009E-40", 0, 0x00, 0x00d573);

    test_bin_op(float_div, 8, 23,
        "0.493176454937537E-19", 0, 0x3e, 0x68e560,
        "0.724506560525941E-19", 0, 0x3f, 0x2b11bc,
        "0.680706679821014",     0, 0x7e, 0x2e42cb);
    // clang-format on
}

TEST(test_float32_add_nan, Suite::add()) {
    // clang-format off
    test_bin_op(float_add, 8, 23,
        "-2",  1, 0x80, 0x000000,
        "NaN", 0, 0xff, 0x400000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_add, 8, 23,
        "NaN", 0, 0xff, 0x400000,
        "-2",  1, 0x80, 0x000000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_add, 8, 23,
        "NaN", 0, 0xff, 0x400000,
        "NaN", 0, 0xff, 0x400000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_add, 8, 23,
        "Inf", 0, 0xff, 0x000000,
        "5",   0, 0x81, 0x200000,
        "Inf", 0, 0xff, 0x000000);
    test_bin_op(float_add, 8, 23,
        "5",   0, 0x81, 0x200000,
        "Inf", 0, 0xff, 0x000000,
        "Inf", 0, 0xff, 0x000000);
    test_bin_op(float_add, 8, 23,
        "NaN", 0, 0xff, 0x400000,
        "Inf", 0, 0xff, 0x000000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_add, 8, 23,
        "Inf", 0, 0xff, 0x000000,
        "NaN", 0, 0xff, 0x400000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_add, 8, 23,
        "Inf", 0, 0xff, 0x000000,
        "Inf", 0, 0xff, 0x000000,
        "Inf", 0, 0xff, 0x000000);
    test_bin_op(float_add, 8, 23,
        "Inf",  0, 0xff, 0x000000,
        "-Inf", 1, 0xff, 0x000000,
        "NaN",  0, 0xff, 0x400000);
    test_bin_op(float_add, 8, 23,
        "-Inf", 1, 0xff, 0x000000,
        "Inf",  0, 0xff, 0x000000,
        "NaN",  0, 0xff, 0x400000);
    // clang-format on
}

TEST(test_float32_mul_plain, Suite::mul()) {
    // clang-format off
    test_bin_op(float_mul, 8, 23,
        "1", 0, 0x7f, 0x000000,
        "1", 0, 0x7f, 0x000000,
        "1", 0, 0x7f, 0x000000);
    test_bin_op(float_mul, 8, 23,
        "1", 0, 0x7f, 0x000000,
        "2", 0, 0x80, 0x000000,
        "2", 0, 0x80, 0x000000);
    test_bin_op(float_mul, 8, 23,
        "2", 0, 0x80, 0x000000,
        "1", 0, 0x7f, 0x000000,
        "2", 0, 0x80, 0x000000);
    test_bin_op(float_mul, 8, 23,
        "2", 0, 0x80, 0x000000,
        "2", 0, 0x80, 0x000000,
        "4", 0, 0x81, 0x000000);
    test_bin_op(float_mul, 8, 23,
        "2",  0, 0x80, 0x000000,
        "-2", 1, 0x80, 0x000000,
        "-4", 1, 0x81, 0x000000);
    test_bin_op(float_mul, 8, 23,
        "-2", 1, 0x80, 0x000000,
        "2",  0, 0x80, 0x000000,
        "-4", 1, 0x81, 0x000000);
    test_bin_op(float_mul, 8, 23,
        "-2", 1, 0x80, 0x000000,
        "-2", 1, 0x80, 0x000000,
        "4",  0, 0x81, 0x000000);
    test_bin_op(float_mul, 8, 23,
        "5",  0, 0x81, 0x200000,
        "-1", 1, 0x7f, 0x000000,
        "-5", 1, 0x81, 0x200000);
    test_bin_op(float_mul, 8, 23,
        "5", 0, 0x81, 0x200000,
        "0", 0, 0x00, 0x000000,
        "0", 0, 0x00, 0x000000);
    test_bin_op(float_mul, 8, 23,
        "0", 0, 0x00, 0x000000,
        "5", 0, 0x81, 0x200000,
        "0", 0, 0x00, 0x000000);

    test_bin_op(float_mul, 8, 23,
        "-0.179871393551936E-18", 1, 0x40, 0x545acb,
        "0.390454969272200E-19",  0, 0x3e, 0x38631c,
        "-0.702316777334955E-38", 1, 0x00, 0x4c79bc);
    // clang-format on
}

TEST(test_float32_mul_nan, Suite::mul()) {
    // clang-format off
    test_bin_op(float_mul, 8, 23,
        "-2",  1, 0x80, 0x000000,
        "NaN", 0, 0xff, 0x400000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_mul, 8, 23,
        "NaN", 0, 0xff, 0x400000,
        "0",   0, 0x00, 0x000000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_mul, 8, 23,
        "NaN", 0, 0xff, 0x400000,
        "NaN", 0, 0xff, 0x400000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_mul, 8, 23,
        "Inf", 0, 0xff, 0x000000,
        "0",   0, 0x00, 0x000000,
        "NaN", 0, 0xff, 0x400000);
    test_bin_op(float_mul, 8, 23,
        "0",   0, 0x00, 0x000000,
        "Inf", 0, 0xff, 0x000000,
        "NaN", 0, 0xff, 0x400000);
    // clang-format on
}

TEST(test_float32_fuzz_add, Suite::add()) {
    fuzz_bin_op<float>(8, 23, "+", float_add, [](auto a, auto b) { return a + b; });
}

TEST(test_float32_fuzz_sub, Suite::add()) {
    fuzz_bin_op<float>(8, 23, "-", float_sub, [](auto a, auto b) { return a - b; });
}

TEST(test_float32_fuzz_mul, Suite::mul()) {
    // 100000 iterations here is to ban solutions with DynNum
    fuzz_bin_op<float>(8, 23, "*", float_mul, [](auto a, auto b) { return a * b; }, 100000);
}

TEST(test_float32_fuzz_div, Suite::div()) {
    fuzz_bin_op<float>(8, 23, "/", float_div, [](auto a, auto b) { return a / b; });
}

TEST(test_float32_parse, Suite::string()) {
    // clang-format off
    test_parse(8, 23, "7813381419433984", 0, 0xb3, 0x5e11d5);
    test_parse(8, 23, "7813381956304896", 0, 0xb3, 0x5e11d6);
    test_parse(8, 23, "0.1498207566328346729278564453125E-3", 0, 0x72, 0x1d1934);
    test_parse(8, 23, "1.4e-45",          0, 0x00, 0x000001);
    test_parse(8, 23, "1.1754944e-38",    0, 0x01, 0x000000);
    // clang-format on
}

TEST(test_float32_to_string, Suite::string()) {
    // clang-format off
    test_to_string(8, 23, "1",  0, 0x7f, 0x000000);
    test_to_string(8, 23, "0",  0, 0x00, 0x000000);
    test_to_string(8, 23, "-1", 1, 0x7f, 0x000000);
    // clang-format on

    compare_with_std_to_string<float>(0, 0xb3, 0x5e11d5); // 7813381419433984
    compare_with_std_to_string<float>(0, 0x00, 0x000001); // 1.4e-45
    compare_with_std_to_string<float>(0, 0x01, 0x000000); // 1.1754944e-38
    compare_with_std_to_string<float>(0, 0xff, 0x000000); // Inf
    compare_with_std_to_string<float>(1, 0xff, 0x000000); // -Inf
    compare_with_std_to_string<float>(0, 0xff, 0x400000); // NaN
}

TEST(test_float32_to_string_fuzz, Suite::string()) { fuzz_to_string<float>(); }

TEST(test_float32_parse_fuzz, Suite::string()) { fuzz_parse<float>(); }