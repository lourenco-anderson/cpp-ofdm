#include <fftw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <complex>

// #include "mySignal.hpp"

class Signal {
    private: 
        std::complex<double> *data;
        size_t length;
    public:
        Signal(size_t length); // contructor: alocate memory
        ~Signal(); // destructor: free memory
        Signal(const Signal &s); // copy constructor
        void print() const;
        bool is_valid() const;
        Signal& operator=(const Signal &other); // copy assignment operator
        Signal(Signal&& other) noexcept; // move constructor
        Signal& operator=(Signal&& other) noexcept; // move assignment operator
};

Signal::Signal(size_t length){
    std::cout << "Signal created" <<std::endl;
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
    std::cout << "Signal destroing" <<std::endl;
    fftw_free(data);
    std::cout << "Signal destroied" <<std::endl;
}

void Signal::print() const{
    for (size_t i = 0; i < length; i++) {
        std::cout << "Value at data["<< i <<"]:" << data[i] << std::endl;
    }
}

bool Signal::is_valid() const {
    return data != NULL;
}

Signal& Signal::operator=(const Signal &other){
    std::cout << "Signal assigned" <<std::endl;
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
    std::cout << "Signal moved" <<std::endl;
    this->data = other.data;
    this->length = other.length;

    other.data = NULL;
    other.length = 0;
}

Signal& Signal::operator=(Signal&& other) noexcept {
    std::cout << "Signal copied by move" << std::endl;
    
    if (this != &other){
        fftw_free(this->data);
        this->data = other.data;
        this->length = other.length;
        other.data = NULL;
        other.length = 0;
    }      
    return *this;
}

int main() {
    static_assert(sizeof(std::complex<double>) == sizeof(fftw_complex), "Incompatible layout between std::complex and fftw_complex");

    size_t num(5);
    // std::cout << "Give me a number:" << std::endl;
    // std::cin >> num;

    Signal s(num);
    if (s.is_valid()){
        std::cout << "...\n" <<std::endl;
        return 1;
    }
    Signal s1(5);
    Signal s2 = s1 ; // copy constructor
    Signal s3(3); // copy constructor
    s3 = s1;
    s.print();

    Signal invalido(0);      // length 0 -> inválido, data = nullptr
    Signal s4(5);             // válido, com memória própria alocada
    s4 = invalido;             // atribuição de um Signal inválido
    std::cout << "s4 valido? " << s4.is_valid() << std::endl;

    Signal s5 = std::move(s1);// move constructor
    std::cout << "s1 valido? " << s1.is_valid() << std::endl;
    std::cout << "s5 valido? " << s5.is_valid() << std::endl;
    
    s3 = std::move(s); // move assignment operator
    std::cout << "s valido? " << s.is_valid() << std::endl;
    std::cout << "s3 valido? " << s3.is_valid() << std::endl;

    s3= std::move(s3); // move assignment operator
    std::cout << "s3 valido? " << s3.is_valid() << std::endl;
    s3.print();


    return 0;
}