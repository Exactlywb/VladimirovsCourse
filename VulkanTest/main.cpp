#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class HelloTriangleApplication {

    const uint32_t width    = 800;
    const uint32_t height   = 600;

    GLFWwindow* window;
    VkInstance  instance;

    VkDebugUtilsMessengerEXT debugMessenger;

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    bool checkValidationLayerSupport () {

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties (&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers (layerCount);
        vkEnumerateInstanceLayerProperties (&layerCount, availableLayers.data ());

        for (const char* layerName: validationLayers) {

            bool layerFound = false;

            for (const auto& layerProperties: availableLayers) {

                if (!strcmp (layerName, layerProperties.layerName)) {

                    layerFound = true;
                    break;

                }

            }

            if (!layerFound)
                return false;

        }

        return true;

    }

    std::vector<const char*> getRequiredExtensions () {

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions (&glfwExtensionCount);

        std::vector<const char*> extensions (glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
            extensions.push_back (VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;

    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (   VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;

    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {

        createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        createInfo.messageSeverity  =   VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType      =   VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     | 
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  | 
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        createInfo.pfnUserCallback = debugCallback;

    }

    void createInstance () {

        if (enableValidationLayers && !checkValidationLayerSupport ())
            throw std::runtime_error ("validation layers requested, but not available!");

        VkApplicationInfo appInfo {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION (1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION (1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions ();
        createInfo.enabledExtensionCount    = static_cast<uint32_t>(extensions.size ());
        createInfo.ppEnabledExtensionNames  = extensions.data ();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        if (enableValidationLayers) {

            createInfo.enabledLayerCount = static_cast<uint32_t> (validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data ();

            populateDebugMessengerCreateInfo (debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

        } else {

            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;

        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
        

    }

    VkResult CreateDebugUtilsMessengerEXT   (VkInstance instance, 
                                            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                            const VkAllocationCallbacks* pAllocator, 
                                            VkDebugUtilsMessengerEXT* pDebugMessenger) {

        auto curFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (curFunc != nullptr)
            return curFunc(instance, pCreateInfo, pAllocator, pDebugMessenger);
        else 
            return VK_ERROR_EXTENSION_NOT_PRESENT;

    }
    void DestroyDebugUtilsMessengerEXT  (VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, 
                                        const VkAllocationCallbacks* pAllocator) {

        auto curFunc = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    
        if (curFunc != nullptr)
            curFunc(instance, debugMessenger, pAllocator);

    }
    void setupDebugMessenger () {

        if (!enableValidationLayers) 
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT (instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
            throw std::runtime_error("failed to set up debug messenger!");

    }

    void initVulkan () {

        createInstance      ();
        setupDebugMessenger ();

    }
    
    void mainLoop () {

        while (!glfwWindowShouldClose (window)) {

            glfwPollEvents ();

        }

    }

    void cleanup () {

        if (enableValidationLayers)
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

        vkDestroyInstance   (instance, nullptr);

        glfwDestroyWindow   (window);
        glfwTerminate       ();

    }

    void initWindow () {

        glfwInit ();

        glfwWindowHint  (GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint  (GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow (width, height, "Vladick - pis'ka", nullptr, nullptr);

    }

public:
    void run () {

        initWindow  ();
        initVulkan  ();

        mainLoop    ();
        cleanup     ();

    }

};

int main () {

    HelloTriangleApplication app;

    app.run ();

    return EXIT_SUCCESS;

}
