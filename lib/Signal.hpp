#include <complex>


// Class signal to represent a signal in time or frequency domain
#ifndef SIGNAL_HPP
#define SIGNAL_HPP

class Signal {
    private: 
        std::complex<double> *data;
        size_t length;
    public:
        Signal(); // Empty constructor
        Signal(size_t length); // constructor: allocate memory
        Signal(size_t length, bool fill_pattern); // constructor: allocate memory and initialize
        ~Signal(); // destructor: free memory
        Signal(const Signal &s); // copy constructor
        void print() const;
        bool is_valid() const;
        size_t size() const;
        std::complex<double> get(size_t i) const; // individual element getter 
        void set(size_t i, std::complex<double> value); // individual element setter
        Signal& operator=(const Signal &other); // copy assignment operator
        Signal(Signal&& other) noexcept; // move constructor
        Signal& operator=(Signal&& other) noexcept; // move assignment operator
        Signal fft() const; // perform FFT on the signal
        Signal ifft() const; // perform iFFT on the signal       
};

#endif