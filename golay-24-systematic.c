
// Copyright (c) 2016-2019 Timur Kristóf
// Licensed to you under the terms of the MIT license.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "golay-24-systematic.h"

/**
 * @file
 * @brief Systematic Golay-24 codec implementation.
 */

#if defined(__arm__)
// clang-format off

/** Lookup table to speed up parity calculation on ARM. */
static uint8_t par_lut[64] = {
    0,   1,   1,   0,   1,   0,   0,   1,
    1,   0,   0,   1,   0,   1,   1,   0,
    1,   0,   0,   1,   0,   1,   1,   0,
    0,   1,   1,   0,   1,   0,   0,   1,
    1,   0,   0,   1,   0,   1,   1,   0,
    0,   1,   1,   0,   1,   0,   0,   1,
    0,   1,   1,   0,   1,   0,   0,   1,
    1,   0,   0,   1,   0,   1,   1,   0,
};
// clang-format on
#endif // __arm__

/**
 * @brief Calculates the popcount (number of ones) of a 32-bit number.
 *
 * This function is used for calculating Hamming-weight, in other words the number of
 * ones (high bits), or the popcount of the given number.
 *
 * On ARM Cortex-M, we observed that the compiler builtin is not very efficient, so we use
 * a special algorithm from "Bit Twiddling Hacks".
 * On other architectures we just use the compiler builtin. For example, on x86,
 * this is compiled to a single instruction, if support is enabled with -mpopcnt in GCC.
 */
__attribute__((always_inline)) static inline uint_fast16_t popcount(uint32_t x)
{
#if defined(__arm__)
    // This generic 32-bit solution is faster on ARM Cortex-M than the builtin.
    // Source: http://graphics.stanford.edu/~seander/bithacks.html

    x = x - ((x >> 1) & 0x55555555);                                    // reuse input as temporary
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);                     // temp
    uint_fast16_t c = (((x + (x >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24; // count

    return (uint_fast16_t) c;
#else
    return __builtin_popcount(x);
#endif
}

/**
 * @brief Calculates the parity of an unsigned 12-bit number.
 *
 * Parity means whether the input number contains an odd or even number of
 * ones (high bits).
 *
 * On ARM Cortex-M, we use a lookup table, which is faster than the compiler builtin.
 * On other architectures, we just use the compiler builtin.
 */
__attribute__((always_inline)) static inline uint_fast16_t parity_12bit(uint_fast16_t x)
{
#if defined(__arm__)
    // Take care of first 6 bits
    uint_fast16_t y = x >> 6;
    x ^= y;

    // Now use the LUT
    x &= 63;
    return par_lut[x];
#else
    return __builtin_parity(x);
#endif
}

/*
    1 0 0 1 1 1 1 1 0 0 0 1;
    0 1 0 0 1 1 1 1 1 0 1 0;
    0 0 1 0 0 1 1 1 1 1 0 1;
    1 0 0 1 0 0 1 1 1 1 1 0;
    1 1 0 0 1 0 0 1 1 1 0 1;
    1 1 1 0 0 1 0 0 1 1 1 0;
    1 1 1 1 0 0 1 0 0 1 0 1;
    1 1 1 1 1 0 0 1 0 0 1 0;
    0 1 1 1 1 1 0 0 1 0 0 1;
    0 0 1 1 1 1 1 0 0 1 1 0;
    0 1 0 1 0 1 0 1 0 1 1 1;
    1 0 1 0 1 0 1 0 1 0 1 1
*/

/**
 * @brief Golay matrix.
 *
 * This is the generator matrix of the systematic Golay-24 code.
 */
static const uint_fast16_t golay_matrix[12] = {0x9F1, 0x4FA, 0x27D, 0x93E, 0xC9D, 0xE4E, 0xF25, 0xF92, 0x7C9, 0x3E6, 0x557, 0xAAB};

/**
 * @brief Multiplies the input with the Golay matrix.
 *
 * The input and output are 12-bit numbers.
 * This function multiplies the input with the Golay matrix.
 */
static inline uint_fast16_t golay_matrix_multiply(uint_fast16_t x)
{
    uint_fast16_t result;

    result = parity_12bit(x & golay_matrix[0]) << 11;
    result |= parity_12bit(x & golay_matrix[1]) << 10;
    result |= parity_12bit(x & golay_matrix[2]) << 9;
    result |= parity_12bit(x & golay_matrix[3]) << 8;
    result |= parity_12bit(x & golay_matrix[4]) << 7;
    result |= parity_12bit(x & golay_matrix[5]) << 6;
    result |= parity_12bit(x & golay_matrix[6]) << 5;
    result |= parity_12bit(x & golay_matrix[7]) << 4;
    result |= parity_12bit(x & golay_matrix[8]) << 3;
    result |= parity_12bit(x & golay_matrix[9]) << 2;
    result |= parity_12bit(x & golay_matrix[10]) << 1;
    result |= parity_12bit(x & golay_matrix[11]);

    return result;
}

/**
 * @brief Encodes a 12-bit message to a systematic Golay-24 codeword.
 */
uint32_t golay_24_systematic_encode(uint_fast16_t x)
{
    uint32_t result = golay_matrix_multiply(x);
    result |= (uint32_t) x << 12;

    return result;
}

/**
 * @brief Decodes a systematic Golay-24 codeword.
 *
 * When the input contains a maximum number of 3 errors, these will be corrected
 * and the original message will be returned. If the input contains 4 errors,
 * this is detected and `GOLAY_24_SYSTEMATIC_ERROR_RESULT` is returned.
 */
uint_fast16_t golay_24_systematic_decode(uint32_t x)
{
    //
    // NOTES:
    // This method uses some tricks from linear algebra together with
    // some special properties of the Golay code. It can be mathematically
    // proven using the following:
    //
    // - the Golay matrix is its own inverse and transpose (therefore columns and rows are equivalent)
    // - the error pattern of one-bit errors is therefore the same as the matrix column selected by the error bit
    // - the Hamming distance between any two codewords is at least 8
    // - since the Golay code can only fix at most 3 errors and detect 4 errors, assume here that there are no more than 4 errors
    //

    // Break the codeword in two halves: data and parity
    uint_fast16_t d = (uint_fast16_t)(x >> 12);
    uint_fast16_t p = (uint_fast16_t)(x & 0x0fff);

    // Calculate parity from the received data
    uint_fast16_t p2 = golay_matrix_multiply(d);

    // If the calculated parity is the same as the received parity, the data is considered intact
    // It could also be that the data contains no errors and the parity contains max. 3 errors
    if ((p2 == p) || (popcount(p ^ p2) <= 3)) {
        return d;
    }

    // Calculate data from the received parity
    uint_fast16_t d2 = golay_matrix_multiply(p);

    // Data contains max 3 errors, parity doesn't contain any errors
    if (popcount(d2 ^ d) <= 3) {
        return d2;
    }

    // Data contains 1 error, parity contains 1 or 2 errors
    for (uint32_t i = 0; i < 12; i++) {
        uint32_t p3 = p2 ^ golay_matrix[i];
        if (popcount(p3 ^ p) <= 2) {
            return d ^ (1 << (11 - i));
        }
    }

    // Parity contains 1 error, data contains 1 or 2 errors
    for (uint32_t i = 0; i < 12; i++) {
        uint32_t d3 = d2 ^ golay_matrix[i];
        if (popcount(d3 ^ d) <= 2) {
            return d3;
        }
    }

    // Too many errors to recover
    return GOLAY_24_SYSTEMATIC_ERROR_RESULT;
}

