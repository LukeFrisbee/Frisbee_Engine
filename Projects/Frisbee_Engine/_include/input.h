#pragma once

#include "GLFW/glfw3.h"

#include "window.h"

namespace fengine {
	class Input {
	public:
		Input(FWindow& window) : m_window(window) {};

		void getMousePos(double& x, double& y) { glfwGetCursorPos(m_window.getGLFWwindow(), &x, &y); }
		int getMouseButton(int button) { return glfwGetMouseButton(m_window.getGLFWwindow(), button); }
		int getKey(int key) { return glfwGetKey(m_window.getGLFWwindow(), key); }

		void setCursorPos(double xPos, double yPos) { glfwSetCursorPos(m_window.getGLFWwindow(), xPos, yPos); }
		
		Input(const Input&) = delete;
		Input(Input&&) = delete;
		Input& operator=(const Input&) = delete;
		Input& operator=(Input&&) = delete;

	private:
		FWindow& m_window;
	};
}