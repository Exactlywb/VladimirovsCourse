#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

#include <map>
#include <optional>
#include <set>

#include <fstream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if 1
static std::vector<char> readFile(const std::string& filename) 
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) 
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
#endif

class FileBuff {

    size_t              fileSize;
    std::vector<char>   buffer;

public:

    FileBuff (const std::string& filename) {

        std::ifstream file (filename, std::ios::ate | std::ios::binary);


        if (!file.is_open ())
            throw std::runtime_error ("bad file open");
        
        fileSize = (size_t)(file.tellg ());
        buffer.resize(fileSize);

        file.seekg  (0);
        file.read   (buffer.data (), fileSize);

        file.close  ();

    }

    const char* GetBuffer () const {
        return buffer.data ();
    }

    size_t GetFileSize () const {
        return fileSize;
    }
    
};

struct Vertex {

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription () {

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;

    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions () {

        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);

        return attributeDescriptions;

    }

};

class HelloTriangleApplication {

    const uint32_t width    = 800;
    const uint32_t height   = 600;

    const int maxFramesInFlight = 2;

    GLFWwindow* window;

    VkInstance  instance;
    VkDevice    device;

    VkQueue     graphicsQueue;
    VkQueue     presentQueue;

    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkSurfaceKHR surface;

    VkRenderPass                    renderPass;
    VkDescriptorSetLayout           descriptorSetLayout;
    VkPipelineLayout                pipelineLayout;
    VkPipeline                      graphicsPipeline;
    std::vector<VkFramebuffer>      swapChainFramebuffers;

    VkCommandPool                   commandPool;
    std::vector<VkCommandBuffer>    commandBuffers;

    std::vector<VkSemaphore>        imageAvailableSemaphores;
    std::vector<VkSemaphore>        renderFinishedSemaphores;

    size_t                          currentFrame = 0;

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

    VkSwapchainKHR              swapChain;
    std::vector<VkImage>        swapChainImages;
    VkFormat                    swapChainImageFormat;
    VkExtent2D                  swapChainExtent;

    std::vector<VkImageView>    swapChainImageViews;

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

     void createDescriptorSetLayout() 
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
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

    VkSurfaceFormatKHR chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& availableFormats) {

        for (const auto& availableFormat : availableFormats) {

            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
            
        }

        return availableFormats [0];
    }

    VkPresentModeKHR chooseSwapPresentMode (const std::vector<VkPresentModeKHR>& availablePresentModes) {

        for (const auto& availablePresentMode : availablePresentModes) {

            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
                return availablePresentMode;
            
        }

        return VK_PRESENT_MODE_FIFO_KHR;

    }

    VkExtent2D chooseSwapExtent (const VkSurfaceCapabilitiesKHR& capabilities) {

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

    void createSwapChain () {

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR  surfaceFormat   = chooseSwapSurfaceFormat (swapChainSupport.formats);
        VkPresentModeKHR    presentMode     = chooseSwapPresentMode (swapChainSupport.presentModes);

        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && 
            imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

         QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {

            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;

        } else
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR (device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
            throw std::runtime_error("failed to create swap chain!");

        vkGetSwapchainImagesKHR (device, swapChain, &imageCount, nullptr);
        swapChainImages.resize (imageCount);
        vkGetSwapchainImagesKHR (device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat    = surfaceFormat.format;
        swapChainExtent         = extent;

    }

    void createImageViews () {

        swapChainImageViews.resize (swapChainImages.size ());

        for (size_t i = 0; i < swapChainImages.size(); ++i) {

            VkImageViewCreateInfo createInfo {};
            
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

            createInfo.image    = swapChainImages [i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format   = swapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel    = 0;
            createInfo.subresourceRange.levelCount      = 1;
            createInfo.subresourceRange.baseArrayLayer  = 0;
            createInfo.subresourceRange.layerCount      = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create image views!");

        }

    }

    void vkFreeSwapChain () {

        for (auto imageView: swapChainImageViews)
            vkDestroyImageView (device, imageView, nullptr);

    }

    //!TODO another class for shader handlings
    VkShaderModule createShaderModule (const FileBuff& shader) {

        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        // std::cout << shader.GetBuffer().size () << std::endl;
        createInfo.codeSize = shader.GetFileSize (); //maybe * sizeof (uint32_t)???

        createInfo.pCode    = reinterpret_cast<const uint32_t*>(shader.GetBuffer ()); // Think about the dead parrot

        std::cout << "\n";
        VkShaderModule shaderModule;
        if (vkCreateShaderModule (device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error ("bad shader module create");
        }

        return shaderModule;

    }

    void buildShaderInfo (const VkShaderModule& vertShaderModule,
                          const VkShaderModule& fragShaderModule,
                          VkPipelineShaderStageCreateInfo shaderStages [2]) {

        
    
    }

    void buildVertexInfo (VkPipelineVertexInputStateCreateInfo& vertexInfo) { //empty

        
        
    }

    void buildAssembly (VkPipelineInputAssemblyStateCreateInfo& inputAssembly) {

        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

    }

    void buildViewport (VkViewport& viewport) {
        
        viewport.x = 0.0f;
        viewport.y = 0.0f;

        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;

        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

    }

    void buildScissor (VkRect2D& scissor) {
        
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;

    }

    void buildViewportInfo  (VkPipelineViewportStateCreateInfo& viewportState,
                            VkViewport& viewport, VkRect2D& scissor) {

        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

    }

    void buildRasterizationInfo (VkPipelineRasterizationStateCreateInfo& rasterizer) {
        
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;

        rasterizer.rasterizerDiscardEnable = VK_FALSE;

        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;

        rasterizer.lineWidth = 1.0f;

        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

        rasterizer.depthBiasEnable = VK_FALSE;
    
    }

    void buildMultisamplingInfo (VkPipelineMultisampleStateCreateInfo& multisampling) {

        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    }

    void buildColorBlend (VkPipelineColorBlendAttachmentState& colorBlendAttachment) {

        colorBlendAttachment.colorWriteMask =   VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
                                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        colorBlendAttachment.blendEnable = VK_FALSE;

    }

    void buildColorBlendInfo (VkPipelineColorBlendStateCreateInfo& colorBlending, VkPipelineColorBlendAttachmentState& colorBlendAttachment) {

        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;

        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

    }

    void buildLayoutInfo (VkPipelineLayoutCreateInfo& pipelineLayoutInfo) {

        
        
    
    }

    void createGraphPipeline () {
        
        FileBuff vertShaderCode {"../vulkan/shaders/vert.spv"};
        FileBuff fragShaderCode {"../vulkan/shaders/frag.spv"};     

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        unsigned long long puhuyPlusPohuy = reinterpret_cast<unsigned long long> (device);        

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        


        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }


    void createRenderPass () {

        VkAttachmentDescription colorAttach {};
        colorAttach.format  = swapChainImageFormat;
        colorAttach.samples = VK_SAMPLE_COUNT_1_BIT;

        colorAttach.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        colorAttach.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttach.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        colorAttach.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttach.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment   = 0;
        colorAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &colorAttachmentRef;

        VkSubpassDependency dependency {};
        dependency.srcSubpass       = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass       = 0;
        dependency.srcStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask    = 0;
        dependency.dstStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType            = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount  = 1;
        renderPassInfo.pAttachments     = &colorAttach;
        renderPassInfo.subpassCount     = 1;
        renderPassInfo.pSubpasses       = &subpass;
        renderPassInfo.dependencyCount  = 1;
        renderPassInfo.pDependencies    = &dependency;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
            throw std::runtime_error("failed to create render pass!");

    }

    void createFramebuffers () {

        swapChainFramebuffers.resize (swapChainImageViews.size ());

        for (size_t i = 0; i < swapChainImageViews.size (); ++i) {

            VkImageView attachments[] = {swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass      = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments    = attachments;
            framebufferInfo.width           = swapChainExtent.width;
            framebufferInfo.height          = swapChainExtent.height;
            framebufferInfo.layers          = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create framebuffer!");

        }

    }

    void vkFreeFramebuffer () {

        for (auto framebuffer : swapChainFramebuffers)
            vkDestroyFramebuffer(device, framebuffer, nullptr);

    }

    void createCommandPool () {

        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) 
            throw std::runtime_error("failed to create graphics command pool!");

    }

    void createCommandBuffers () {
        
        commandBuffers.resize(swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo {};

        allocInfo.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool   = commandPool;

        allocInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount    = (uint32_t) commandBuffers.size();
        
        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffers!");

        for (size_t i = 0; i < commandBuffers.size (); ++i) {
            
            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType        = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            
            renderPassInfo.renderPass   = renderPass;
            renderPassInfo.framebuffer  = swapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChainExtent;

            VkClearValue clearColor = {0.063f, 0.163f, 0.088f, 1.0f};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers [i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline   (commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            vkCmdDraw           (commandBuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass  (commandBuffers[i]);

            if (vkBeginCommandBuffer (commandBuffers [i], &beginInfo) != VK_SUCCESS)
                throw std::runtime_error("failed to begin recording command buffer!");
        
        }
    
    }

    void createSemaphores () {

        imageAvailableSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (size_t i = 0; i < maxFramesInFlight; i++)
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create semaphores for a frame!");
 
    }

    void vkDestroySemaphores (VkDevice& device) {

        for (size_t i = 0; i < maxFramesInFlight; i++) {

            vkDestroySemaphore (device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore (device, imageAvailableSemaphores[i], nullptr);

        }

    }

    void initVulkan () {

        createInstance          ();
        
        setupDebugMessenger     ();

        createSurface           ();

        pickPhysicalDevice      (instance); //!TODO maybe it's better not to transfer arguments

        createLogicalDevice     ();
        

        createSwapChain         ();


        createImageViews        ();
        
        createRenderPass        ();
        createDescriptorSetLayout();
        createGraphPipeline     ();
        createFramebuffers      ();

        createCommandPool       ();
        createCommandBuffers    (); //!TODO
        createSemaphores        ();

    }

    void drawFrame () {
        
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR (device, swapChain, UINT64_MAX, imageAvailableSemaphores [currentFrame], 
                                                VK_NULL_HANDLE, &imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores []       = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages []  = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.pWaitDstStageMask    = waitStages;

        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &commandBuffers [imageIndex];

        VkSemaphore signalSemaphores [] = {renderFinishedSemaphores [currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");
        
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR   (presentQueue, &presentInfo);
        vkQueueWaitIdle     (presentQueue);

    }
    
    void mainLoop () {

        while (!glfwWindowShouldClose (window)) {

            glfwPollEvents  ();
            drawFrame       ();

        }

    }

    void cleanup () {

        if (enableValidationLayers)
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        
        vkDestroySemaphores (device);

        vkDestroyCommandPool    (device, commandPool, nullptr);

        vkFreeFramebuffer       ();
        vkDestroyPipeline       (device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout (device, pipelineLayout  , nullptr);
        vkDestroyRenderPass     (device, renderPass      , nullptr);

        vkFreeSwapChain         ();

        vkDestroyDevice         (device, nullptr);
        vkDestroySurfaceKHR     (instance, surface, nullptr);
        vkDestroySwapchainKHR   (device, swapChain, nullptr);
        vkDestroyInstance       (instance, nullptr);

        glfwDestroyWindow       (window);
        glfwTerminate           ();

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
