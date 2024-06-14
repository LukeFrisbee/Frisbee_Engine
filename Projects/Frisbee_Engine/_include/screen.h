#pragma once

#include "window.h"

namespace fengine {
	class Screen {
	public:
		Screen(FWindow& window) : m_window(window) {};

		int getWidth() { return m_window.getExtent().width; }
		int getHeight() { return m_window.getExtent().height; }

		Screen(const Screen&) = delete;
		Screen(Screen&&) = delete;
		Screen& operator=(const Screen&) = delete;
		Screen& operator=(Screen&&) = delete;
	private:
		FWindow& m_window;
	};
}