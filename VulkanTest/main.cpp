#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

#include <map>
#include <optional>
#include <set>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class HelloTriangleApplication {

    const uint32_t width    = 800;
    const uint32_t height   = 600;

    GLFWwindow* window;

    VkInstance  instance;
    VkDevice    device;

    VkQueue     graphicsQueue;
    VkQueue     presentQueue;

    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkSurfaceKHR surface;

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    struct QueueFamilyIndices {

        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete () {
            return graphicsFamily.has_value() && presentFamily.has_value ();
        }

    };

    struct SwapChainSupportDetails {

        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;

    };

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

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            throw std::runtime_error("failed to create instance!");

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

    bool checkDeviceExtensionSupport (VkPhysicalDevice device) {
        
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties (device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions (extensionCount);
        vkEnumerateDeviceExtensionProperties (device, nullptr, &extensionCount, availableExtensions.data ());

        std::set<std::string> requiredExtensions (deviceExtensions.begin (), deviceExtensions.end ());

        for (const auto& extension: availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty ();

    }

    bool isDeviceSuitable (VkPhysicalDevice device) {

        QueueFamilyIndices indices  = findQueueFamilies (device);
        bool extensionsSupported    = checkDeviceExtensionSupport (device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {

            SwapChainSupportDetails swapChainSupport = querySwapChainSupport (device);
            swapChainAdequate = !swapChainSupport.formats.empty () && !swapChainSupport.presentModes.empty ();

        }

        return indices.isComplete () && extensionsSupported && swapChainAdequate;

    }

    void pickPhysicalDevice (VkInstance instance) {

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0)
            throw std::runtime_error("failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, VkPhysicalDevice> candidates;
        for (const auto& device : devices) {

            int score = rateDeviceSuitability(device);
            if (score > 0)
                candidates.insert(std::make_pair(score, device));

        }

        // Check if the best candidate is suitable at all
        if (candidates.rbegin()->first > 0)
            physicalDevice = candidates.rbegin()->second;
        else
            throw std::runtime_error("failed to find a suitable GPU!");

    }

    QueueFamilyIndices findQueueFamilies (VkPhysicalDevice device) {

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int queueFamIndex = 0;
        for (const auto& queueFamily : queueFamilies) {

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamily = queueFamIndex;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR (device, queueFamIndex, surface, &presentSupport);
            if (presentSupport)
                indices.presentFamily = queueFamIndex;

            if (indices.isComplete())
                break;

            ++queueFamIndex;

        }

        return indices;

    }

    int rateDeviceSuitability(VkPhysicalDevice device) {

        if (!findQueueFamilies (device).isComplete()) return -1;

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        int score = 0;

        //Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader)
            return 0;

        return score;
        
    }

    void createLogicalDevice () {

        QueueFamilyIndices indices = findQueueFamilies (physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {

            VkDeviceQueueCreateInfo queueCreateInfo{};

            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

            queueCreateInfo.queueFamilyIndex = queueFamily;

            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);

        }

        VkPhysicalDeviceFeatures deviceFeatures {};

        VkDeviceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size ());
        createInfo.pQueueCreateInfos    = queueCreateInfos.data ();

        createInfo.pEnabledFeatures     = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size ());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data ();

        if (enableValidationLayers) {

            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size ());
            createInfo.ppEnabledLayerNames = validationLayers.data ();

        } else
            createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
            throw std::runtime_error("failed to create logical device!");

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    }

    void createSurface () {

        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("failed to create window surface!");
        
    }

    SwapChainSupportDetails querySwapChainSupport (VkPhysicalDevice device) {

        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR (device, surface, &details.capabilities);
        
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR (device, surface, &formatCount, nullptr);

        if (formatCount != 0) {

            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR (device, surface, &formatCount, details.formats.data());

        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR (device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {

            details.presentModes.resize (presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR (device, surface, &presentModeCount, details.presentModes.data ());

        }

        return details;

    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

        for (const auto& availableFormat : availableFormats) {

            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
            
        }

        return availableFormats [0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

        for (const auto& availablePresentMode : availablePresentModes) {

            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
                return availablePresentMode;
            
        }

        return VK_PRESENT_MODE_FIFO_KHR;

    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

        if (capabilities.currentExtent.width != UINT32_MAX)
            return capabilities.currentExtent;
        else {

            int width   = 0; 
            int height  = 0;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;

        }

    }

    void createSwapChain() {

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR  surfaceFormat   = chooseSwapSurfaceFormat (swapChainSupport.formats);
        VkPresentModeKHR    presentMode     = chooseSwapPresentMode (swapChainSupport.presentModes);

        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && 
            imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;

        //!TODO till not ended

    }

    void initVulkan () {

        createInstance      ();
        
        setupDebugMessenger ();

        createSurface       ();

        pickPhysicalDevice  (instance); //!TODO maybe it's better not to transfer arguments
        createLogicalDevice ();

        createSwapChain     ();

    }
    
    void mainLoop () {

        while (!glfwWindowShouldClose (window)) {

            glfwPollEvents ();

        }

    }

    void cleanup () {

        if (enableValidationLayers)
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

        vkDestroyDevice     (device, nullptr);
        vkDestroySurfaceKHR (instance, surface, nullptr);
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
