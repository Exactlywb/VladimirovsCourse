#ifndef VULKAN_HPP__
#define VULKAN_HPP__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>

#include "octree.hpp"

namespace dblCmpTeamGraphLib
{

    class TrApplication;

    static const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    static const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    #ifdef NDEBUG
    const bool enableValidationLayers = false;
    #else
    static const bool enableValidationLayers = true;
    #endif

    static VkResult CreateDebugUtilsMessengerEXT    (VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                                    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (func)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    struct QueueFamilyIndices final {

        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {

            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails final {

        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct Vertex final {

        glm::vec3 pos;
        glm::vec3 color;
        glm::vec3 normal;

        static VkVertexInputBindingDescription getBindingDescription() {

            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {

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

    

    struct UniformBufferObject final {

        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
        alignas(16) glm::vec3 view_pos;
    };
 
    class Window final {
    private:

        int WIDTH_;
        int HEIGHT_;

        GLFWwindow* window_;

    public:


        Window (uint32_t width, uint32_t height, const std::string nameWindow = "Vulkan") : WIDTH_(width), HEIGHT_(height) { //problem with string?? ref? //NOT THIS THIS

            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            window_ = glfwCreateWindow(WIDTH_, HEIGHT_, nameWindow.data(), nullptr, nullptr); //width and height to class and construct it
            
            glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
        } 

        GLFWwindow* getWindow () const {return window_;}

        Window (const Window& rhs)              = delete;   // ban copy
        Window (Window&& rhs)                   = delete;   // ban move

        Window &operator= (Window&& rhs)        = delete;   // ban move assignment
        Window &operator= (const Window& rhs)   = delete;   // ban copy assignment

       ~Window () {

            glfwDestroyWindow(window_);
            glfwTerminate();
       } 

        static void framebufferResizeCallback (GLFWwindow* window, int WIDTH, int HEIGHT);

        static void key_callback                (GLFWwindow* window , int key, int scancode, int action, int mods);
        static void cursor_position_callback    (GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback       (GLFWwindow* window, int button, int action, int mods) noexcept;

    };

    class TrApplication final {
    public:

        TrApplication () : workSpace {600, 800, "Vulcan"} {

            glfwSetWindowUserPointer(workSpace.getWindow(), this);

        }


        TrApplication (const TrApplication& rhs)              = delete;   // ban copy
        TrApplication (TrApplication&& rhs)                   = delete;   // ban move

        TrApplication &operator= (TrApplication&& rhs)        = delete;   // ban move assignment
        TrApplication &operator= (const TrApplication& rhs)   = delete;   // ban copy assignment

        ~TrApplication();

        void run() {

            initVulkan();
            mainLoop();

        }

        bool framebufferResized_ = false;


    private:
        Window workSpace; //instead of it there must be class //think about inheritance

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkCommandPool commandPool;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        void drawFrame(); //public or private

        void initVulkan() {
            
            createInstance();

            setupDebugMessenger();

            createSurface();

            pickPhysicalDevice();
            createLogicalDevice();

            createSwapChain();
            createImageViews();
            createRenderPass();
            createDescriptorSetLayout();
            createGraphicsPipeline();
            createFramebuffers();

            createCommandPool();
            createVertexBuffer();
            createIndexBuffer();
            createUniformBuffers();

            createDescriptorPool();
            createDescriptorSets();
            createCommandBuffers();
            
            createSyncObjects();

        }

        void mainLoop();

        void cleanupSwapChain();

        void recreateSwapChain();

        void createInstance();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {

            createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = debugCallback;
        }

        void setupDebugMessenger() {

            if (!enableValidationLayers) 
                return;

            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            populateDebugMessengerCreateInfo(createInfo);

            if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) 
                throw std::runtime_error("failed to set up debug messenger!");
        }

        void createSurface() {

            if (glfwCreateWindowSurface(instance, workSpace.getWindow(), nullptr, &surface) != VK_SUCCESS)
                throw std::runtime_error("failed to create window surface!");
        }

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createSwapChain();

        void createImageViews();

        void createRenderPass();

        void createDescriptorSetLayout();

        void createGraphicsPipeline();

        void createFramebuffers();

        void createCommandPool();

        void createVertexBuffer();

        void createIndexBuffer();

        void createUniformBuffers();

        void createDescriptorPool();

        void createDescriptorSets();

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void createCommandBuffers();

        void createSyncObjects();

        void updateUniformBuffer(uint32_t currentImage);

        VkShaderModule createShaderModule(const std::vector<char>& code);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) noexcept;

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) noexcept;

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        std::vector<const char*> getRequiredExtensions();

        bool checkValidationLayerSupport();

        static std::vector<char> readFile(const std::string& filename);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(__attribute__((unused))VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
                                                            __attribute__((unused))VkDebugUtilsMessageTypeFlagsEXT messageType, 
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
                                                            __attribute__((unused))void* pUserData) {

            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }
    };
}


void    drawTriangles   (const std::vector<GObjects::Triangle>& trianglesArr, bool* flagArr);

#endif
