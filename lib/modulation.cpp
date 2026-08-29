#include <complex>
#include "modulation.hpp"

Signal qpsk_modulate (const std::vector<int>& bits){
    Signal out;
    if (bits.size() % 2 == 0) {
        out = Signal(bits.size() / 2, false);

        for (size_t i = 0; i<out.size(); i++){
            int bit_1 = bits[2*i];
            int bit_2 = bits[2*i + 1];

            if (bit_1 == 0 && bit_2 == 0) {
                out.set(i, std::complex<double>(1, 1)); // 00 -> 1 + i
            } else if (bit_1 == 0 && bit_2 == 1) {
                out.set(i, std::complex<double>(-1, 1)); // 01 -> -1 + i
            } else if (bit_1 == 1 && bit_2 == 0) {
                out.set(i, std::complex<double>(-1, -1)); // 10 -> -1 - i
            } else if (bit_1 == 1 && bit_2 == 1) {
                out.set(i, std::complex<double>(1, -1)); // 11 -> 1 - i
            }

        }   
    }
    return out;
}