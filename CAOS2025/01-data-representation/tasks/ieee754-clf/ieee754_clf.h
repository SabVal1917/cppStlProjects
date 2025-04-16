#pragma once

typedef enum {
    Zero = 0x00,
    MinusZero = 0x01,
    Inf = 0xF0,
    MinusInf = 0xF1,
    Regular = 0x10,
    MinusRegular = 0x11,
    Denormal = 0x20,
    MinusDenormal = 0x21,
    NaN = 0x30
} float_class_t;

float_class_t classify(double value);
