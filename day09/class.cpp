#include <iostream>

class Signal {
    private: 
        double *data;
        size_t length;
    public:
        Signal(size_t length); // contructor: alocate memory
        ~Signal(); // destructor: free memory
        Signal(const Signal &s); // copy constructor
        void print() const;
        bool is_valid() const;
        Signal& operator=(const Signal &other); // copy assignment operator
};

Signal::Signal(size_t length){
    this->length = length;

    if (length == 0){
        std::cout << "Invalid number. Exiting.\n"<<std::endl;
        this->data = NULL; 
    }else{
        data = new double[length];
        for (size_t i = 0; i < length; i++){
            if (i%2 == 0){
                this->data[i] = 1;
            }else{
                this->data[i] = -1;
            }
        }
    }   
}

Signal::Signal(const Signal &s){
    if (s.is_valid()){
        this->length = s.length;
        this->data = new double[s.length];

        for (size_t i = 0; i<s.length; i++){
            this->data[i] = s.data[i];
        }
    }else{
        this->length = 0;
        this->data = NULL;
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

Signal& Signal::operator=(const Signal &other){
    if (this != &other){
        delete[] this->data;

        if (other.is_valid()){
            this->data = new double[other.length];
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

int main() {
    size_t num(5);
    // std::cout << "Give me a number:" << std::endl;
    // std::cin >> num;

    Signal s(num);
    if (s.is_valid() == 0){
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

    return 0;
}