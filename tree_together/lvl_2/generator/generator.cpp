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

    int elemNum = 0;
    std::cin >> elemNum;

    std::cout << elemNum << " ";

    std::vector<int> vec;

    int maxGenNum = 0;
    for (int i = 0; i < elemNum; ++i) {

        int genNum = std::round (distr (generator));
        if (genNum > maxGenNum)
            maxGenNum = genNum;

        std::cout << genNum << " ";
    }

//REQUESTS
    int requestNum = 0;
    std::cin >> requestNum;

    std::cout << requestNum << " ";

    std::uniform_int_distribution<int> reqDistr {-maxGenNum, maxGenNum};

    for (int i = 0; i < requestNum; ++i)
        std::cout << reqDistr (generator) << " " << reqDistr (generator) << " ";

}