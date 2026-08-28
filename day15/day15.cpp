#include <fftw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <complex>
#include <cassert>

class Signal {
    private: 
        std::complex<double> *data;
        size_t length;
    public:
        Signal(); // Empty constructor
        Signal(size_t length); // constructor: allocate memory
        // Signal(size_t length, bool fill_pattern); // constructor: allocate memory and initialize
        ~Signal(); // destructor: free memory
        Signal(const Signal &s); // copy constructor
        void print() const;
        bool is_valid() const;
        std::complex<double> get(size_t i) const; // individual element getter 

        Signal& operator=(const Signal &other); // copy assignment operator
        Signal(Signal&& other) noexcept; // move constructor
        Signal& operator=(Signal&& other) noexcept; // move assignment operator

        Signal fft() const; // perform FFT on the signal
        Signal ifft() const; // perform iFFT on the signal
        
};

Signal::Signal(){
    this->length = 0;
    this->data = NULL;
}

Signal::Signal(size_t length){
    // std::cout << "Signal created" <<std::endl;
    this->length = length;

    if (length == 0){
        std::cout << "Invalid number. Exiting.\n"<<std::endl;
        this->data = NULL; 
    }else{
        data = static_cast<std::complex<double>*>(fftw_malloc(sizeof(std::complex<double>) * length));
        if (data == NULL) {
            std::cerr << "Memory allocation failed. Exiting.\n" << std::endl;
            this->length = 0;
        } else {
           for (size_t i = 0; i < length; i++){
                if (i%2 == 0){
                    this->data[i] = std::complex<double>(1, 0);
                }else{
                    this->data[i] = std::complex<double>(-1, 0);
                }
            }
        }
        
    }   
}

Signal::Signal(const Signal &s){
    std::cout << "Signal copied" <<std::endl;
    if (s.is_valid()){
        this->length = s.length;
        this->data = static_cast<std::complex<double>*>(fftw_malloc(sizeof(std::complex<double>) * s.length));

        for (size_t i = 0; i<s.length; i++){
            this->data[i] = s.data[i];
        }
    }else{
        this->length = 0;
        this->data = NULL;
    }
}

Signal::~Signal(){
    // std::cout << "Signal destroing" <<std::endl;
    fftw_free(data);
    // std::cout << "Signal destroied" <<std::endl;
}

void Signal::print() const{
    for (size_t i = 0; i < length; i++) {
        std::cout << "Value at data["<< i <<"]:" << data[i] << std::endl;
    }
}

bool Signal::is_valid() const {
    return data != NULL;
}

std::complex<double> Signal::get(size_t i) const{
    return data[i];
}

Signal& Signal::operator=(const Signal &other){
    // std::cout << "Signal assigned" <<std::endl;
    if (this != &other){
        fftw_free(this->data);

        if (other.is_valid()){
            this->data = static_cast<std::complex<double>*>(fftw_malloc(sizeof(std::complex<double>) * other.length));
            for (size_t i = 0; i < other.length; i++){
                this->data[i] = other.data[i];
            }
            this->length = other.length;
        }else{
            this->data = NULL;
            this->length = 0;
        }       
    }      
    return *this;
}

Signal::Signal(Signal&& other) noexcept {
    // std::cout << "Signal moved" <<std::endl;
    this->data = other.data;
    this->length = other.length;

    other.data = NULL;
    other.length = 0;
}

Signal& Signal::operator=(Signal&& other) noexcept {
    // std::cout << "Signal copied by move" << std::endl;
    
    if (this != &other){
        fftw_free(this->data);
        this->data = other.data;
        this->length = other.length;
        other.data = NULL;
        other.length = 0;
    }      
    return *this;
}

Signal Signal::ifft() const {
    Signal out;

    out.length = this->length;
    out.data = static_cast<std::complex<double>*>(fftw_malloc(sizeof(std::complex<double>) * out.length));

    fftw_plan p = fftw_plan_dft_1d(this->length, reinterpret_cast<fftw_complex*>(this->data), reinterpret_cast<fftw_complex*>(out.data), FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    for (size_t i = 0; i<out.length; i++){
        out.data[i] = out.data[i]/static_cast<double>(out.length);
    }

    fftw_destroy_plan(p);
    return out;
}

Signal Signal::fft() const {
    Signal out;

    out.length = this->length;
    out.data = static_cast<std::complex<double>*>(fftw_malloc(sizeof(std::complex<double>) * out.length));

    fftw_plan p = fftw_plan_dft_1d(this->length, reinterpret_cast<fftw_complex*>(this->data), reinterpret_cast<fftw_complex*>(out.data), FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    // Not normalized fft() as in the fftw libreary
    return out;
}

int main() {
    static_assert(sizeof(std::complex<double>) == sizeof(fftw_complex), "Incompatible layout between std::complex and fftw_complex");   

    const size_t N = 8; // Size of the input signal
    Signal in(N);
    std::cout << "Signal in time domain:" << std::endl;
    in.print();
    
    std::cout << "Signal in frequency domain:" << std::endl;
    Signal freq = in.fft();   
    freq.print();

    std::cout << "Signal reconverted to time domain" << std::endl;
    Signal recovered = freq.ifft();
    recovered.print();


    for (size_t i = 0; i < N; i++){
        if(std::abs(recovered.get(i) - in.get(i))>1e-9){
            assert(std::abs(recovered.get(i) - in.get(i)) < 1e-9);
        }
    }

    fftw_cleanup();
    return 0;
}