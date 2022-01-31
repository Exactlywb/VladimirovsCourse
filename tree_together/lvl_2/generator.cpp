#include <iostream>
#include <chrono>
#include <random>
#include <map>

int main ()
{
    unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count ();
    std::mt19937 generator (seed);

    std::uniform_real_distribution<double> distr{-15, 15};

    for (int i = 0; i < 30; ++i)
        std::cout << std::round(distr(generator)) << " ";
}