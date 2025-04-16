#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "ieee754_clf.h"

void format_float_class(float_class_t c, char buf[], size_t len) {
    switch (c) {
        case Zero:
            snprintf(buf, len, "Zero");
            return;
        case MinusZero:
            snprintf(buf, len, "MinusZero");
            return;
        case Inf:
            snprintf(buf, len, "Inf");
            return;
        case MinusInf:
            snprintf(buf, len, "MinusInf");
            return;
        case Regular:
            snprintf(buf, len, "Regular");
            return;
        case MinusRegular: 
            snprintf(buf, len, "MinusRegular");
            return;
        case Denormal:
            snprintf(buf, len, "Denormal");
            return;
        case MinusDenormal:
            snprintf(buf, len, "MinusDenormal");
            return;
        case NaN:
            snprintf(buf, len, "NaN");
            return;
        default:
            snprintf(buf, len, "???");
            return;
    }
}

bool test_clf(double x, float_class_t expected_class) {
    float_class_t actual_class = classify(x);
    if (expected_class == actual_class) {
        return true;
    }

    char expected_str[64];
    format_float_class(expected_class, expected_str, sizeof(expected_str));
    char actual_str[64];
    format_float_class(actual_class, actual_str, sizeof(actual_str));
    fprintf(
        stderr,
        "Test failed:\n"
        "  expected classify(%f) = %s\n"
        "  got classify(%f) = %s\n",
        x, expected_str, x, actual_str);

    return false;
}

bool run_all_tests() {
    const int kTests = 9;

    int tests_passed = 0;
    tests_passed += test_clf(0.0, Zero);
    tests_passed += test_clf(-0.0, MinusZero);
    tests_passed += test_clf(123.456, Regular);
    tests_passed += test_clf(-123.456, MinusRegular);
    tests_passed += test_clf(1.0 / 0.0, Inf);
    tests_passed += test_clf(-1.0 / 0.0, MinusInf);
    tests_passed += test_clf(1e-315, Denormal);
    tests_passed += test_clf(-1e-315, MinusDenormal);
    tests_passed += test_clf(0.0 / 0.0, NaN);
    printf("Passed %d/%d tests\n", tests_passed, kTests);

    return tests_passed == kTests;
}

int main() {
    if (run_all_tests()) {
        return 0;
    } else {
        return 1;
    }
}
