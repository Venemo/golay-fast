
// This file is part of fecmagic, the forward error correction library.
// Copyright (c) 2016 Timur Kristóf
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

#ifndef BINARYPRINT_H
#define BINARYPRINT_H

#include <iostream>
#include <type_traits>

namespace fecmagic {

    // Forward-declaration of the BinaryPrint class
    template <typename T, uint32_t B>
    class BinaryPrint;
    
}

// Forward-declaration of the stream operators for BinaryPrint
template <typename T, uint32_t B>
::std::ostream &operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<T, B> &bp);

namespace fecmagic {

    /**
     * Convenience class for printing integers in binary form.
     */
    template<typename T = uint8_t, uint32_t B = (sizeof(T) * 8)>
    class BinaryPrint {
    private:
        // Friend: generic stream operator
        template<typename X, uint32_t Y>
        friend ::std::ostream &::operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<X, Y> &bp);
        
        // Wrapped value
        T val;
        
        // Separator
        ::std::string separator;
        
    public:
        /**
         * Constructs an instance of the BinaryPrint class.
         */
        inline explicit BinaryPrint(const T &v, std::string sep = std::string())
            : val(v), separator(sep) { }
        
    };

}

/**
 * Prints a BinaryPrint instance into a standard stream.
 */
template<typename T, uint32_t B>
inline ::std::ostream &operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<T, B> &bp) {
    // Current bit index
    uint32_t x = 0;
    // Go through all bits with a mask
    for (T i = (1 << (sizeof(T) * 8 - 1)); i; i >>= 1) {
        // If appropriate, print a separator
        if ((x % 8 == 0) && (x != 0)) {
            os << bp.separator;
        }
        
        // Print current bit
        os << ((bp.val & i) ? "1" : "0");
        // Increase current bit index
        x++;
    }
    
    return os;
}

#endif // BINARYPRINT_H

