#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Float {
    // TODO
};

typedef struct Float Float;

int float_init(Float* self, int exponent_bits, int mantissa_bits);
void float_destroy(Float* self);

int float_get_exponent_bits(Float* self);
int float_get_mantissa_bits(Float* self);

bool float_get_sign(const Float* self);
void float_get_exponent(const Float* self, void* target);
void float_get_mantissa(const Float* self, void* target);

void float_set_sign(Float* self, bool sign);
void float_set_exponent(Float* self, const void* exponent);
void float_set_mantissa(Float* self, const void* mantissa);

void float_add(Float* result, const Float* a, const Float* b);
void float_sub(Float* result, const Float* a, const Float* b);
void float_mul(Float* result, const Float* a, const Float* b);
void float_div(Float* result, const Float* a, const Float* b);

void float_next(Float* self);
void float_prev(Float* self);

void float_parse(Float* self, const char* string);
int float_string(const Float* self, char* string, int n);

#ifdef __cplusplus
}
#endif