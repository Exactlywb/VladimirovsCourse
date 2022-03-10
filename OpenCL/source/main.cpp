#include "bitSort.hpp"
#include <algorithm>
#include <chrono>

std::vector<int>    ReadData ();
void                PrintRes (std::vector<int>::const_iterator st,
                              std::vector<int>::const_iterator fin);

int main () {

    cl::vector<cl::Platform> platforms = OpenCLApp::App::getNewPlatform ();
    std::vector<std::pair<cl::Platform, cl::Device>> devices = OpenCLApp::App::getDevices (platforms.begin (), platforms.end ());

    cl::Device workWith = devices [0].second; //!TODO remake
    std::vector<int> data = ReadData ();

    try {

        OpenCLApp::BitSortApp bitSort (workWith);
        bitSort.run (data);

        PrintRes (data.begin (), data.end ());

    } catch (cl::Error &err) {

        std::cerr << err.what () << std::endl;

    }

}

void PrintRes (std::vector<int>::const_iterator st, std::vector<int>::const_iterator fin) {

    for (; st != fin; ++st)
        std::cout << (*st) << std::endl;

}

std::vector<int> ReadData () {

    int amount = 0;
    std::cin >> amount;

    if (!std::cin.good ())
        throw std::runtime_error ("Bad read from stdin");
    
    std::vector<int> res;
    for (int i = 0; i < amount; ++i) {

        int num = 0;
        std::cin >> num;
        if (!std::cin.good ())
            throw std::runtime_error ("Bad read from stdin");

        res.emplace_back (num);

    }

    return res;

}
