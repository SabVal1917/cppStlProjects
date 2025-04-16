/*
 * https://github.com/paweldac/source_location/blob/master/include/source_location/source_location.hpp
 *
 * MIT License
 *
 * Copyright (c) 2019 paweldac
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef NOSTD_SOURCE_LOCATION_HPP
#define NOSTD_SOURCE_LOCATION_HPP

#pragma once

#include <cstdint>

// Clang
#if defined(__clang__) && !defined(__apple_build_version__) && (__clang_major__ >= 9)
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FILE
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FUNCTION
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_LINE
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_COLUMN
// AppleClang https://en.wikipedia.org/wiki/Xcode#Toolchain_versions
#elif defined(__apple_build_version__) && defined(__clang__) && (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__ % 100) >= 110003
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FILE
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FUNCTION
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_LINE
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_COLUMN
// GCC
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FILE
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FUNCTION
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_LINE
#define NOSTD_SOURCE_LOCATION_NO_BUILTIN_COLUMN
// MSVC https://github.com/microsoft/STL/issues/54#issuecomment-616904069 https://learn.microsoft.com/en-us/cpp/overview/compiler-versions?view=msvc-170
#elif defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER) && (_MSC_VER >= 1926)
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FILE
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FUNCTION
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_LINE
#define NOSTD_SOURCE_LOCATION_HAS_BUILTIN_COLUMN
#endif

namespace nostd {
struct source_location {
public:
#if defined(NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FILE) && defined(NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FUNCTION) && defined(NOSTD_SOURCE_LOCATION_HAS_BUILTIN_LINE) && defined(NOSTD_SOURCE_LOCATION_HAS_BUILTIN_COLUMN)
    static constexpr source_location current(const char* fileName = __builtin_FILE(),
        const char* functionName = __builtin_FUNCTION(),
        const uint_least32_t lineNumber = __builtin_LINE(),
        const uint_least32_t columnOffset = __builtin_COLUMN()) noexcept
#elif defined(NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FILE) && defined(NOSTD_SOURCE_LOCATION_HAS_BUILTIN_FUNCTION) && defined(NOSTD_SOURCE_LOCATION_HAS_BUILTIN_LINE) && defined(NOSTD_SOURCE_LOCATION_NO_BUILTIN_COLUMN)
    static constexpr source_location current(const char* fileName = __builtin_FILE(),
        const char* functionName = __builtin_FUNCTION(),
        const uint_least32_t lineNumber = __builtin_LINE(),
        const uint_least32_t columnOffset = 0) noexcept
#else
    static constexpr source_location current(const char* fileName = "unsupported",
        const char* functionName = "unsupported",
        const uint_least32_t lineNumber = 0,
        const uint_least32_t columnOffset = 0) noexcept
#endif
    {
        return source_location(fileName, functionName, lineNumber, columnOffset);
    }

    constexpr source_location() noexcept = default;

    constexpr const char* file_name() const noexcept
    {
        return fileName;
    }

    constexpr const char* function_name() const noexcept
    {
        return functionName;
    }

    constexpr uint_least32_t line() const noexcept
    {
        return lineNumber;
    }

    constexpr std::uint_least32_t column() const noexcept
    {
        return columnOffset;
    }

private:
    constexpr source_location(const char* fileName, const char* functionName, uint_least32_t lineNumber,
        uint_least32_t columnOffset) noexcept
        : fileName(fileName)
        , functionName(functionName)
        , lineNumber(lineNumber)
        , columnOffset(columnOffset)
    {
    }

    const char* fileName = "";
    const char* functionName = "";
    std::uint_least32_t lineNumber {};
    std::uint_least32_t columnOffset {};
};
} // namespace nostd

#endif