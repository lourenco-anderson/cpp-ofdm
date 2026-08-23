#include <fftw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <complex>

int main() {
    static_assert(sizeof(std::complex<double>) == sizeof(fftw_complex), "Incompatible layout between std::complex and fftw_complex");
    const int N = 8; // Size of the input signal
    const int k0 = 2; // Frequency index to analyze

    // Create an input signal
    // fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    // fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    std::vector<std::complex<double>> in(N), out(N);

    for (int n=0; n < N; ++n) {
        double angle = 2.0 * M_PI * k0 * n / N;
        // in[n][0] = cos(angle); // Real part
        // in[n][1] = sin(angle); // Imaginary part
        in[n] = std::complex<double>(cos(angle), sin(angle));
    }

    fftw_plan p = fftw_plan_dft_1d(N, reinterpret_cast<fftw_complex*>(in.data()), reinterpret_cast<fftw_complex*>(out.data()), FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    // Print the FFT output
    std::cout << "FFT Output:" << std::endl;
    for (int k=0; k < N; ++k) {
        std::cout << "bin " << k << ": "
                  << out[k] << std::endl;
    }

    // fftw_complex *test = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    // fftw_plan p_test = fftw_plan_dft_1d(N, out, test, FFTW_BACKWARD, FFTW_ESTIMATE);
    // fftw_execute(p_test);

    // // Print the inverse FFT output
    // std::cout << "Inverse FFT Output:" << std::endl;
    // for (int n=0; n < N; ++n) {
    //     std::cout << "sample " << n << ": "
    //               << test[n][0] / N << " + " << test[n][1] / N << "i" << std::endl;
    // }
    
    // fftw_destroy_plan(p_test);
    // fftw_free(test);
    fftw_destroy_plan(p);
    // fftw_free(in);
    // fftw_free(out);
    
}