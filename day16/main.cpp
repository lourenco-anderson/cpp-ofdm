#include <fftw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <complex>
#include <cassert>


// Class signal to represent a signal in time or frequency domain

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
        std::complex<double> get(size_t i) const; // individual element getter 
        void set(size_t i, std::complex<double> value); // individual element setter
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

Signal::Signal(size_t length, bool fill_pattern){
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
            if (fill_pattern) {
                for (size_t i = 0; i < length; i++) {
                    if (i % 2 == 0) {
                        this->data[i] = std::complex<double>(1, 0);
                    } else {
                        this->data[i] = std::complex<double>(-1, 0);
                    }
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

void Signal::set(size_t i, std::complex<double> value){
    data[i] = value;
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


// Class matrix to represent a matrix in time or frequency domain

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

    return 0;

}