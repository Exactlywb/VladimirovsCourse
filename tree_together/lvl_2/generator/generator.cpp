#include <iostream>
#include <chrono>
#include <random>
#include <map>

#include <vector>

int main ()
{
    unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count ();
    std::mt19937 generator (seed);

    double dispr;
    std::cin >> dispr;

    std::normal_distribution<double> distr{0, dispr};

    int elemNum;
    std::cin >> elemNum;

    std::vector<int> requests;

    for (int i = 0; i < elemNum; ++i) {
        int genNum = std::round (distr (generator));
        requests.push_back(genNum);
    }

    size_t vecSize = requests.size ();
    std::cout << vecSize << " ";

    for (int i = vecSize - 1; i >= 0; --i)
        std::cout << requests [(long unsigned int)i] << " ";

    std::normal_distribution<double> distrReq {0, dispr / 20};

    std::cout << vecSize << " ";
    for (size_t i = 0; i < vecSize; ++i) 
        std::cout << std::round (distrReq (generator)) << " " << std::round (distrReq (generator)) << " ";
}