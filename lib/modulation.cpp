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

Matrix ofdm_modulate(const std::vector<int>& input, size_t num_subcarriers){
    // Assume que o tamanho do input é múltiplo de num_subcarriers
    Matrix out;
    size_t total_modulated_symbols = input.size() / 2;
    if (total_modulated_symbols % num_subcarriers != 0){
        return out;
    }else{
        size_t num_ofdm_symbols = total_modulated_symbols / num_subcarriers;
        Signal modulated_signal = qpsk_modulate(input);
        Matrix temp(num_ofdm_symbols, num_subcarriers);

        for (size_t i = 0; i < num_ofdm_symbols; i++){
            for (size_t j = 0; j < num_subcarriers; j++){
                temp.set(i, j, modulated_signal.get(i * num_subcarriers + j));
            }
        }

        out = Matrix(num_ofdm_symbols, num_subcarriers);
        for (size_t i = 0; i <num_ofdm_symbols; i++){
            Signal row = temp.get_row(i);
            Signal ifft_row = row.ifft();
            out.set_row(i, ifft_row);
        }

        return out;
    }
}