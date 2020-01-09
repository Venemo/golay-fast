
#include <iostream>
#include <limits>
#include <chrono>
#include <unistd.h>
#include "../golay-24-systematic.h"
#include "bitmaskcombination.h"
#include "binaryprint.h"

using namespace std;
using namespace fecmagic;

typedef uint32_t (*GolayEncoder)(uint_fast16_t);
typedef uint_fast16_t (*GolayDecoder)(uint32_t);

inline void assert_dec(uint32_t is, uint32_t should)
{
    if (is != should) {
        cout << "Can't decode." << endl << "Encoded: " << should << ", decoded: " << is << endl;
        exit(1);
    }
}

inline void assert_dec_err(uint32_t is, uint32_t should, uint32_t encoded, uint32_t err)
{
    if (is != should) {
        cout << "Can't decode." << endl << "Encoded: " << should << " -> " << encoded << ", error=" << BinaryPrint<uint32_t>(err) << ", decoded: " << is << endl;
        exit(1);
    }
}

void test_impl(GolayEncoder encode, GolayDecoder decode, uint32_t detected_err_result)
{
    // Encode all possible inputs
    
    constexpr size_t num_inputs = 4096;
    uint32_t encoded[num_inputs] = { 0 };
    uint32_t count;
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    int elapsed;
    
    cout << "Encode: start, " << num_inputs << "inputs" << endl;
    begin = std::chrono::steady_clock::now();

    for (uint16_t i = 0; i < num_inputs; i++) {
        uint32_t x = encode(i);
        encoded[i] = x;
    }
    
    end = std::chrono::steady_clock::now();
    elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    cout << "Encode: took " << elapsed << " ms" << endl;
    
    // No errors
    
    cout << "Decode: 0 errors: start" << endl;
    begin = std::chrono::steady_clock::now();
    count = 0;
    for (uint16_t i = 0; i < num_inputs; i++) {
        uint32_t x = encoded[i];
        uint16_t y = decode(x);
        
        assert_dec(y, i);
        count++;
    }
    
    end = std::chrono::steady_clock::now();
    elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    cout << "Decode: 0 errors: took " << elapsed << " ms" << endl;
    cout << "Decode: 0 errors: everything can be decoded: " << count << endl;
    
    // 1-bit errors
    
    cout << "Decode: 1-bit errors: start" << endl;
    begin = std::chrono::steady_clock::now();
    
    count = 0;
    for (uint16_t i = 0; i < num_inputs; i++) {
        uint32_t x = encoded[i];
        uint32_t e;
        fecmagic::BitmaskCombination<uint32_t, 1, 24> errs;
        
        while ((e = errs.next())) {
            uint16_t y = decode(x ^ e);
            assert_dec_err(y, i, x, e);
            count++;
        }
    }
    
    end = std::chrono::steady_clock::now();
    elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    cout << "Decode: 1-bit errors:took " << elapsed << " ms" << endl;
    cout << "Decode: 1-bit errors: everything can be decoded: " << count << endl;
    
    // 2-bit errors
    
    cout << "Decode: 2-bit errors: start" << endl;
    begin = std::chrono::steady_clock::now();
    
    count = 0;
    for (uint16_t i = 0; i < num_inputs; i++) {
        uint32_t x = encoded[i];
        uint32_t e;
        fecmagic::BitmaskCombination<uint32_t, 2, 24> errs;
        
        while ((e = errs.next())) {
            //cout << fecmagic::BinaryPrint<uint32_t, 24>(e) << endl;
            uint16_t y = decode(x ^ e);
            assert_dec_err(y, i, x, e);
            count++;
        }
    }
    
    end = std::chrono::steady_clock::now();
    elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    cout << "Decode: 2-bit errors: took " << elapsed << " ms" << endl;
    cout << "Decode: 2-bit errors: everything can be decoded: " << count << endl;
    
    // 3-bit errors
    
    cout << "Decode: 3-bit errors: start" << endl;
    begin = std::chrono::steady_clock::now();
    
    count = 0;
    for (uint16_t i = 0; i < num_inputs; i++) {
        uint32_t x = encoded[i];
        uint32_t e;
        fecmagic::BitmaskCombination<uint32_t, 3, 24> errs;
        
        while ((e = errs.next())) {
            uint16_t y = decode(x ^ e);
            assert_dec_err(y, i, x, e);
            count++;
        }
    }
    
    end = std::chrono::steady_clock::now();
    elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    cout << "Decode: 3-bit errors: took " << elapsed << " ms" << endl;
    cout << "Decode: 3-bit errors: everything can be decoded: " << count << endl;
    
    
    // 4-bit errors
    cout << "Decode: 4-bit errors: start" << endl;
    begin = std::chrono::steady_clock::now();
    
    uint32_t detected = 0;
    uint32_t fixed = 0;
    count = 0;
    for (uint16_t i = 0; i < num_inputs; i++) {
        uint32_t x = encoded[i];
        uint32_t e;
        fecmagic::BitmaskCombination<uint32_t, 4, 24> errs;
        
        while ((e = errs.next())) {
            uint_fast16_t y = decode(x ^ e);
            count++;
            if (y == i) {
                fixed++;
            }
            else if (y == detected_err_result) {
                detected++;
            }
        }
    }
    
    end = std::chrono::steady_clock::now();
    elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    cout << "Decode: 4-bit errors: took " << elapsed << " ms" << endl;    
    cout << "Decode: 4-bit errors: done, total: " << count << " fixed: " << fixed << " (" << ((double)fixed / (double)count * 100.0) << "%) " << " detected: " << detected << " failed: " << (count - fixed - detected) << endl;

}

int main() {
    cout << "-----" << endl << "Fast systematic Golay-24 decoder test" << endl;
    test_impl(golay_24_systematic_encode, golay_24_systematic_decode, GOLAY_24_SYSTEMATIC_ERROR_RESULT);
    
    return 0;
}

