#include <vector>
#include "Signal.hpp"
#include "Matrix.hpp"

Signal qpsk_modulate (const std::vector<int>& bits);

std::vector<int> qpsk_demodulate(const Signal& symbols);

Matrix ofdm_modulate(const std::vector<int>& input, size_t num_subcarriers);

std::vector<int> ofdm_demodulate(const Matrix& time_domain, size_t num_subcarriers);
