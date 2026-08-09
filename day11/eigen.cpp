#include <iostream>
#include <Eigen/Dense>

int main(){
    // 1st contact with Eigen
    // Eigen::MatrixXd m(2,3);
    // m << 1, 2, 3,
    //      4, 5, 6;
    // double* raw = m.data();
    // for (int i = 0; i < 6; i++) {
    //     std::cout << raw[i] << " ";
    // }
    // std::cout << std::endl;

    // // Row-major vs Column-major behavior
    // double raw[6] = {1, 2, 3, 4, 5 ,6};

    // Eigen::Map<Eigen::MatrixXd> WrongMap(raw, 2,3);
    // std::cout<<"Wrong Map: \n" << WrongMap <<std::endl;

    // Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> CorrectMap(raw,2,3);
    // std::cout<<"Correct Map: \n" << CorrectMap <<std::endl;

    // LinAlg usage

    Eigen::MatrixXd A(2,2);
    A << 2, 1,
         1, 3;

    Eigen::VectorXd b(2);
    b << 5, 10;

    Eigen::VectorXd x = A.colPivHouseholderQr().solve(b);
    std::cout << x << std::endl;
}