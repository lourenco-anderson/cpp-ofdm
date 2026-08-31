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
                out.set(i, std::complex<double>(-1, -1)); // 00 -> -1 - i
            } else if (bit_1 == 0 && bit_2 == 1) {
                out.set(i, std::complex<double>(-1, 1)); // 01 -> -1 + i
            } else if (bit_1 == 1 && bit_2 == 0) {
                out.set(i, std::complex<double>(1, -1)); // 10 -> 1 - i
            } else if (bit_1 == 1 && bit_2 == 1) {
                out.set(i, std::complex<double>(1, 1)); // 11 -> 1 + i
            }

        }   
    }
    return out;
}

std::vector<int> qpsk_demodulate(const Signal& symbols){
    size_t num_symbols = symbols.size(); 
    std::vector<int> out(2*num_symbols);

    for (size_t i = 0; i < num_symbols; i++){
        if(symbols.get(i).real() >= 0){
            out[2*i] = 1;
        }else{
            out[2*i] = 0;
        }
        if(symbols.get(i).imag() >= 0){
            out[2*i + 1] = 1;
        }else{
            out[2*i + 1] = 0;
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

std::vector<int> ofdm_demodulate(const Matrix& time_domain, size_t num_subcarriers){
    std::vector<int> out;
    if (time_domain.is_valid()){
        size_t num_ofdm_symbols = time_domain.size_rows();
        
        for (size_t i = 0; i< num_ofdm_symbols; i++){
            Signal row_i = time_domain.get_row(i);
            Signal fft_row_i = row_i.fft();
            std::vector<int> temp_bits = qpsk_demodulate(fft_row_i);
            out.insert(out.end(), temp_bits.begin(), temp_bits.end());
        }  
        
        return out;
    }else{
        return out;
    }
}

