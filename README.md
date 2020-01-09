# Fast golay encoder and decoder

A fast encoder and decoder for the systematic Golay-24 code.

A simple test case is provided which demonstrates that the decoder produces the correct
output for every possible input. (There are only 4096 possible inputs, so this is easy
to verify.)

Created by Timur Kristóf, used on board the SMOG-P/SMOG-1/ATL-1 satellites.

## Encoding

We simply store the Golay generator matrix in memory and use that to implement
binary matrix multiplication using bitwise instructions.

## Decoding

Traditionally, Golay decoders used a syndrome-based decoding algorithm. The algorithm
used here is something else entirely.

1. Split the received codewords into two parts: received message and received parity.
2. Calculate the parity of the received message.
3. See if received message is correct, and received parity contains at most 3 errors.
   If the Hamming distance of the calculated parity and received parity is less than
   or equal to 3, the received message is correct, and it is the decoded code word.
4. Calculate the message from the received parity.
5. See if received parity is correct, and received message contains at most 3 errors.
   If the Hamming distance of the calculated message and received message is less than
   or equal to 3, the received parity is correct, and the calculated message is the
   decoded code word.
6. See if received message contains 1 error and received parity contains 1 or 2 errors.
   Basically, try to fix every possible 1-bit error in the received message, and see if
   the Hamming distance of the received parity and the corrected parity is less than
   or equal to 2.
7. See if received message contains 1 or 2 errors and received parity contains 1 error.
   Try to fix every possible 1-bit error in the received parity, and see if
   the Hamming distance of the received message and the corrected message is less than
   or equal to 2.
8. If we reached this point, we detected an unfixable 4-bit error.


### When FEC is not used for error detection

#### "Best guess" decoding of 4-bit errors

When you don't need the 4-bit error detection capability of the Golay-24 code, it is
possible to modify the algorithm as follows:

* Change **step 6-7** to use *less than or equal to 3* instead of 2.
* Change the final step to just return the received message part as the decoded code word.

The resulting decoder will lose the ability to detect 4-bit errors, but
gain the ability to "correctly" decode **16%** of the possible 4-bit error combinations.

#### Implementing Golay-23 code

When you don't need the 4-bit error detection capability of the Golay-24 code, and
need to make sure the encoded code words are as small as possible, you can
drop one bit from the output of the encoder in order to produce a Golay-23 code word.

To decode a Golay-23 code word, the decoder needs to:

* Add a 0 bit to the input, and try to decode it as systematic Golay-24
* If a 4-bit error is detected, flip the added bit to 1

The resulting code will lose the ability to detect 4-bit errors, but
the encoded code words will occupy less space. This is perfectly acceptable in
situations when you already have a different scheme in place to ensure message integrity.
