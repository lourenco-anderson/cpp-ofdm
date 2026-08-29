#include <fftw3.h>  
#include <iostream>
#include <cassert>
#include <complex>
#include <vector>

#include "Signal.hpp"
#include "Matrix.hpp"
#include "modulation.hpp"

int main() {
    static_assert(sizeof(std::complex<double>) == sizeof(fftw_complex), "Incompatible layout between std::complex and fftw_complex");   

    size_t r(5), c(5);

    Matrix m(r, c);
    if(m.is_valid()){
        std::cout << "Matrix created with " << r << " rows and " << c << " columns." << std::endl;
        m.print();
    }else{
        std::cout << "Matrix creation failed." << std::endl;    
        return 1;
    }
    Matrix m1(4,4);
    Matrix m2 = m1;

    Matrix m3(3,3);
    m3 = m1;

    Matrix invalid(0,0);
    Matrix m4(r,c);
    m4 = invalid;

    std::cout << "m4 valido? " << m4.is_valid() << std::endl;

    Matrix m5 = std::move(m1);// move constructor
    std::cout << "m5 valido? " << m5.is_valid() << std::endl;
    std::cout << "m1 valido? " << m1.is_valid() << std::endl;

    m3 = std::move(m1); // move assignment operator
    std::cout << "m3 valido? " << m3.is_valid() << std::endl;
    std::cout << "m1 valido? " << m1.is_valid() << std::endl;

    m3= std::move(m3); // move assignment operator
    std::cout << "m3 valido? " << m3.is_valid() << std::endl;
    m3.print();

    std::cout << "Row 2 of matrix m:" << std::endl;
    Signal row = m.get_row(2);
    row.print();

    std::vector<int> bits = {0, 0, 0, 1, 1, 0, 1, 1};
    Signal qpsk = qpsk_modulate(bits);
    qpsk.print();
    return 0;

}