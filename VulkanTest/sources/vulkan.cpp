#include "vulkan.hpp"

namespace dblCmpTeamGraphLib {

    void HelloTriangleApplication::drawFrame() 
    {
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            recreateSwapChain();
            return;
        }
        
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(imageIndex);

        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) 
        {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }

        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized_) 
        {
            framebufferResized_ = false;
            recreateSwapChain();
        }
        
        else if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Window::framebufferResizeCallback (GLFWwindow* window, int WIDTH, int HEIGHT) {

        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->framebufferResized_ = true;
    }

    void Window::key_callback (GLFWwindow* window, int key, int scancode, int action, int mods) { 

        float cam_speed = 0.05f;

        if (key == GLFW_KEY_RIGHT) {

            glm::vec3 tmp_vec = glm::normalize (glm::cross (camera_direction, camera_up)) * cam_speed;
            camera_pos += tmp_vec;
        }
        else if (key == GLFW_KEY_LEFT) {

            glm::vec3 tmp_vec = glm::normalize (glm::cross (camera_direction, camera_up)) * cam_speed;
            camera_pos -= tmp_vec;
        }

        if (key == GLFW_KEY_UP)
            camera_pos += camera_direction * cam_speed;
        else if (key == GLFW_KEY_DOWN)
            camera_pos -= camera_direction * cam_speed;
    }

    void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

        if (lpress) {

            double delta_x = xpos - prev_x;
            double delta_y = ypos - prev_y;

            prev_x = xpos;
            prev_y = ypos;

            double sensivity = 0.001;

            phi -= delta_x * sensivity;
            ksi -= delta_y * sensivity;

            camera_direction = glm::vec3 (glm::cos (ksi) * glm::cos (phi), glm::cos (ksi) * glm::sin (phi), glm::sin (ksi));
        }
        else {
            
            prev_x = xpos;
            prev_y = ypos;
        }
    }

}