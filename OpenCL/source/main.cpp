#include "bitSort.hpp"
#include <algorithm>
#include <chrono>

int main () {

    cl::vector<cl::Platform> platforms = OpenCLApp::App::getNewPlatform ();
    std::vector<std::pair<cl::Platform, cl::Device>> devices = OpenCLApp::App::getDevices (platforms.begin (), platforms.end ());

    for (auto d: devices)
        std::cout << d.second.getInfo<CL_DEVICE_NAME> () << std::endl; 

}

