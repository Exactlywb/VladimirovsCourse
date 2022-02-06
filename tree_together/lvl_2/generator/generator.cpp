#include <iostream>
#include <chrono>
#include <random>
#include <map>

#include <vector>

int main ()
{
    unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count ();
    std::mt19937 generator (seed);

    double dispr = 10;
    std::cin >> dispr;

    std::normal_distribution<double> distr{0, dispr};

    int elemNum = 0;
    std::cin >> elemNum;

    // std::cout << elemNum << " ";

    std::vector<int> requests;

    for (int i = 0; i < elemNum; ++i) {

        int genNum = std::round (distr (generator));
        requests.push_back(genNum);
        // std::cout << genNum << " ";
    }

//REQUESTS
    // int requestNum = 0;
    // std::cin >> requestNum;

    // std::cout << requestNum << " ";

    // std::normal_distribution<double> reqDistr {0, dispr};

    // for (int i = 0; i < requestNum; ++i) {
    //     int min = std::round (reqDistr (generator));
    //     int max = std::round (reqDistr (generator));

    //     if (min > max)
    //         std::swap (min, max);

    //     std::cout << min << " " << max << " ";
    // }

    size_t vecSize = requests.size ();
    std::cout << vecSize << " ";

    for (int i = vecSize - 1; i >= 0; --i) {

        std::cout << requests [(long unsigned int)i] << " ";

    }

    std::cout << vecSize / 2 << " ";
    for (size_t i = 0; i < vecSize; ++i)
        std::cout << requests [i] << " ";

} 