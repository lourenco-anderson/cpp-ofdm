#include <fftw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <complex>
#include <cassert>

#include "Matrix.hpp"

Matrix::Matrix(){
    this->rows = 0;
    this->cols = 0;
    this->data = NULL;
}

Matrix::Matrix(size_t rows, size_t cols){
    this->rows = rows;
    this->cols = cols;
    if (rows == 0 || cols == 0){
        std::cout << "Invalid number. Exiting.\n"<<std::endl;
        this->data = NULL;
        this->rows = 0;
        this->cols = 0;
    }else{
        data = static_cast<std::complex<double>*>(fftw_malloc(sizeof(std::complex<double>) * rows * cols));
        if (data == NULL) {
            std::cerr << "Memory allocation failed. Exiting.\n" << std::endl;
            this->rows = 0;
            this->cols = 0;
        } else {
            for (size_t i = 0; i < rows; i++){
                for (size_t j = 0; j < cols; j++){
                    data[i * cols + j] = std::complex<double>(i * cols + j, 0);
                }
            }
        }
    }
}

Matrix::~Matrix(){
    fftw_free(data);
}

Matrix::Matrix(const Matrix &other){
    if (other.is_valid()){
        this->rows = other.rows;
        this->cols = other.cols;
        this->data = static_cast<std::complex<double>*>(fftw_malloc(sizeof(std::complex<double>) * other.rows * other.cols));
        for (size_t i = 0; i < other.rows; i++){
            for (size_t j = 0; j < other.cols; j++){
                this->data[i * other.cols + j] = other.data[i * other.cols + j];
            }
        }
    }else{
        this->rows = 0;
        this->cols = 0;
        this->data = NULL;
    }
}

void Matrix::print() const{
    for (size_t i = 0; i < rows; i++){
        for (size_t j = 0; j < cols; j++){
            std::cout << "Value at data["<< i <<"]["<< j <<"]:" << data[i * cols + j] << std::endl;
        }
    }
}

bool Matrix::is_valid() const{
    return data != NULL;
}

std::complex<double> Matrix::get(size_t i, size_t j) const{
    return data[i * cols + j];
}

void Matrix::set(size_t i, size_t j, std::complex<double> value){
    data[i * cols + j] = value;
}   

Matrix& Matrix::operator=(const Matrix &other){
    if (this != &other){
        fftw_free(this->data);

        if (other.is_valid()){
            this->data = static_cast<std::complex<double>*>(fftw_malloc(sizeof(std::complex<double>) * other.rows * other.cols));
            for (size_t i = 0; i < other.rows; i++){
                for (size_t j = 0; j < other.cols; j++){
                    this->data[i * other.cols + j] = other.data[i * other.cols + j];
                }
            }
            this->rows = other.rows;
            this->cols = other.cols;
        }else{
            this->data = NULL;
            this->rows = 0;
            this->cols = 0;
        }
    }
    return *this;
}

Matrix::Matrix(Matrix&& other) noexcept{
    this->data = other.data;
    this->rows = other.rows;
    this->cols = other.cols;

    other.data = NULL;
    other.rows = 0;
    other.cols = 0;
} 

Matrix& Matrix::operator=(Matrix&& other) noexcept{
    if (this != &other){
        fftw_free(this->data);
        this->data = other.data;
        this->rows = other.rows;
        this->cols = other.cols;
        other.data = NULL;
        other.rows = 0;
        other.cols = 0;
    }
    return *this;
}

Signal Matrix::get_row(size_t i) const{
    Signal row(this->cols, false);

    for (size_t j = 0; j < cols; j++){
        row.set(j, this->data[i * cols + j]);
    }
    return row;
}


void Matrix::set_row(size_t i, const Signal& row){
    for (size_t j = 0; j < cols; j++){
        this->data[i * cols + j] = row.get(j);
    }
}