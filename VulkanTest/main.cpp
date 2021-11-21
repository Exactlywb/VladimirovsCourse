#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class HelloTriangleApplication {

    GLFWwindow* window;
    VkInstance  instance;

    void createInstance () {

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

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = nullptr;

        glfwExtensions = glfwGetRequiredInstanceExtensions (&glfwExtensionCount);

        createInfo.enabledExtensionCount    = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames  = glfwExtensions;

        createInfo.enabledLayerCount = 0;

        VkResult result = vkCreateInstance (&createInfo, nullptr, &instance);

    }

    void initVulkan () {

        createInstance ();

    }
    
    void mainLoop () {

        while (!glfwWindowShouldClose (window)) {

            glfwPollEvents ();

        }

    }

    void cleanup () {

        glfwDestroyWindow (window);
        glfwTerminate ();

    }

    void initWindow () {

        glfwInit ();

        glfwWindowHint  (GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint  (GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow (800, 600, "Vladick - pis'ka", nullptr, nullptr);

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
