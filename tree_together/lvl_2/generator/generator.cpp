#include <iostream>
#include <chrono>
#include <random>
#include <map>

#include <vector>

int main ()
{
    unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count ();
    std::mt19937 generator (seed);

    std::normal_distribution<double> distr{0, 10};


    std::vector<int> vec;

    for (int i = 0; i < 100000; ++i)
        std::cout << std::round(distr(generator)) << " ";
}