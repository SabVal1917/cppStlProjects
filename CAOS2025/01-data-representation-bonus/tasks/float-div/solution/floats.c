
#include "floats.h"

#ifdef __cplusplus
extern "C" {
#endif

int float_init(Float* self, int exponent_bits, int mantissa_bits) {
    // TODO
    return 0;
}

void float_destroy(Float* self) {
    // TODO
}

int float_get_exponent_bits(Float* self) {
    // TODO
    return 0;
}

int float_get_mantissa_bits(Float* self) {
    // TODO
    return 0;
}

bool float_get_sign(const Float* self) {
    // TODO
    return false;
}

void float_get_exponent(const Float* self, void* target) {
    // TODO
}

void float_get_mantissa(const Float* self, void* target) {
    // TODO
}

void float_set_sign(Float* self, bool sign) {
    // TODO
}

void float_set_exponent(Float* self, const void* exponent) {
    // TODO
}

void float_set_mantissa(Float* self, const void* mantissa) {
    // TODO
}

void float_add(Float* result, const Float* a, const Float* b) {
    // TODO
}

void float_sub(Float* result, const Float* a, const Float* b) {
    // TODO
}

void float_mul(Float* result, const Float* a, const Float* b) {
    // TODO
}

void float_div(Float* result, const Float* a, const Float* b) {
    // TODO
}

void float_next(Float* self) {
    // TODO
}

void float_prev(Float* self) {
    // TODO
}

void float_parse(Float* self, const char* string) {
    // TODO
}

int float_string(const Float* self, char* string, int n) {
    // TODO
    return 0;
}

#ifdef __cplusplus
}
#endif