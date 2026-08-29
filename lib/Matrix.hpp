#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <complex>

#include "Signal.hpp"

// Class signal to represent a signal in time or frequency domain

class Matrix {
    private:
        std::complex<double> *data;
        size_t rows;
        size_t cols;
    public:
        Matrix(); // Empty constructor
        Matrix(size_t rows, size_t cols); // constructor: allocate memory
        ~Matrix(); // destructor: free memory
        Matrix(const Matrix &other); // copy constructor
        std::complex<double> get(size_t i, size_t j) const; // individual element getter
        void set(size_t i, size_t j, std::complex<double> value); // individual element setter
        void print() const; // print the matrix
        bool is_valid() const; // check if the matrix is valid

        Matrix& operator=(const Matrix &other); // copy assignment operator
        Matrix(Matrix&& other) noexcept; // move constructor
        Matrix& operator=(Matrix&& other) noexcept; // move assignment operator

        Signal get_row(size_t i) const;

};


#endif