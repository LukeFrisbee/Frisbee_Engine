#pragma once

#define GLFW_INCLUDE_VULKAN
#pragma once

#include "GLFW/glfw3.h"
#include <string>

namespace fengine {
class FWindow
{
	public:
		FWindow(int width, int height, std::string name);
		~FWindow();

		FWindow(const FWindow&) = delete;
		FWindow& operator = (const FWindow&) = delete;

		bool shouldClose() const { return glfwWindowShouldClose(m_glfwWindow); }
		VkExtent2D getExtent() const { return { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) }; }
		bool wasWindowResized() const { return m_framebufferResized; }
		void resetWindowResizedFlag() { m_framebufferResized = false; }

		GLFWwindow* getGLFWwindow() const { return m_glfwWindow; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		std::string windowName;
		GLFWwindow* m_glfwWindow;

		int m_width;
		int m_height;
		bool m_framebufferResized = false;

		void initWindow();
		static void framebufferResizedCallback(GLFWwindow* glfwWindow, int width, int height);
};
}