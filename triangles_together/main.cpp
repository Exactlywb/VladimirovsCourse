#include "include/vector.hpp"

int main () {

    GObjects::Vector<double> myVector {1, 2, 1};
    std::cout << myVector << std::endl;

    std::cin >> myVector;
    
    std::cout << myVector << std::endl;
    std::cout << -myVector << std::endl;
    std::cout << ((-myVector) ^ myVector) << std::endl;
    std::cout << ((-myVector) * myVector) << std::endl;
    std::cout << myVector.squareLength () << std::endl;

    return 0;

}
