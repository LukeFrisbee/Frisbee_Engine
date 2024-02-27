#include "window.h"

#include <stdexcept>

#include <imgui_impl_glfw.h>

namespace fengine {

	FWindow::FWindow(int width, int height, std::string name)
		: m_width{ width }, m_height{ height }, windowName{ name } 
	{
		initWindow();
	}

	void FWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_glfwWindow = glfwCreateWindow(m_width, m_height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_glfwWindow, this);
		glfwSetFramebufferSizeCallback(m_glfwWindow, framebufferResizedCallback);
	}

	FWindow::~FWindow()
	{
		glfwDestroyWindow(m_glfwWindow);
		glfwTerminate();
	}

	void FWindow::framebufferResizedCallback(GLFWwindow* glfwWindow, int width, int height)	
	{
		auto window = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(glfwWindow));
		window->m_framebufferResized = true;
		window->m_width = width;
		window->m_height = height;
	}

	void FWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_glfwWindow, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}
}