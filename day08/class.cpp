#include <iostream>

class Signal {
    private: 
        double *data;
        size_t length;
    public:
        Signal(size_t length); // contructor: alocate memory
        ~Signal(); // destructor: free memory
        void print() const;
        bool is_valid() const;
};

Signal::Signal(size_t length){
    this->length = length;

    if (length == 0){
        std::cout << "Invalid number. Exiting.\n"<<std::endl;
        this->data = NULL; 
    }else{
        data = new double[length];

        if (this->data == NULL){
            std::cout << "Memory allocation failed. Exiting.\n"<<std::endl;
            this->length = 0;
        }else{
            for (size_t i = 0; i < length; i++){
                if (i%2 == 0){
                    this->data[i] = 1;
                }else{
                    this->data[i] = -1;
                }
            }
        }        
    }   
}

Signal::~Signal(){
    delete[] data;
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

int main() {
    size_t num;
    std::cout << "Give me a number:" << std::endl;
    std::cin >> num;

    Signal s(num);
    if (s.is_valid() == 0){
        std::cout << "...\n" <<std::endl;
        return 1;
    }

    s.print();

    return 0;
}