#ifndef BIT_SORT_H__
#define BIT_SORT_H__

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <CL/cl2.hpp>
#include <iostream>
#include <utility>
#include <fstream>

namespace OpenCLApp {

    class App {

        cl::Device          device_;
        cl::Context         context_;
        cl::CommandQueue    queue_;
    
    public:
        using clPlatformVecIt = cl::vector<cl::Platform>::const_iterator;

        static cl::vector<cl::Platform> getNewPlatform () {

            cl::vector<cl::Platform> platforms;
            cl::Platform::get (&platforms);
            //!TODO if platforms.size () == 0 then exception

            return platforms;

        }

        static cl::Context getNewContext (const cl::Device& device) {

            if (device == cl::Device {})    //!TODO own exceptions
                throw std::runtime_error ("bad device");

            return cl::Context {device};

        }
        static cl::CommandQueue getNewQueue (const cl::Context& context, 
                                             const cl::Device& device) {
            return cl::CommandQueue {context, device};  //!TODO catch error
        }

        static std::vector<std::pair<cl::Platform, cl::Device>> getDevices (App::clPlatformVecIt st, 
                                                                            App::clPlatformVecIt fin) {
            std::vector<std::pair<cl::Platform, cl::Device>> output;
            for (; st != fin; ++st) {

                std::vector<cl::Device> devices;
                
                try {
                    (*st).getDevices (CL_DEVICE_TYPE_ALL, &devices);
                } catch (cl::Error& err) {
                    //!TODO what do I have to do it? Ask Khronos...
                }

                for (auto&& d: devices) {

                    if (d.getInfo<CL_DEVICE_AVAILABLE> ())
                        output.emplace_back (*st, d);

                }

            }

            return output;

        }

        App (const cl::Device& device):
                device_ (device),
                context_ (getNewContext (device)), 
                queue_ (getNewQueue (context_, device))
        {}

    };

}

#endif

