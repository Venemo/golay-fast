
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

#ifndef GOLAY_24_SYSTEMATIC_H
#define GOLAY_24_SYSTEMATIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GOLAY_24_SYSTEMATIC_ERROR_RESULT 65535

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @file
 * @brief Systematic Golay-24 codec header.
 */

uint32_t golay_24_systematic_encode(uint_fast16_t x);

uint_fast16_t golay_24_systematic_decode(uint32_t x);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GOLAY_24_SYSTEMATIC_H

