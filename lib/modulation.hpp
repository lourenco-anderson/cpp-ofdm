#include <vector>
#include "Signal.hpp"
#include "Matrix.hpp"

Signal qpsk_modulate (const std::vector<int>& bits);

Matrix ofdm_modulate(const std::vector<int>& input, size_t num_subcarriers);