#pragma once

#include <chrono>
#include <vector>

#include "device.h"
#include "window.h"
#include "renderer.h"
#include "render_object.h"
#include "render_system.h"
#include "script.h"
#include "input.h"
#include "screen.h"
#include "camera.h"

#include "render_object_holder.h"
#include "model_builder.h"

namespace fengine { 
	class App {

	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 1200;

		App();
		void run();

		App(const App &) = delete;
		App& operator=(const App&) = delete;

	private:
		void _loadGameObjects();
		void _renderLoop(RenderObject& cameraObject);
		void _time(std::chrono::steady_clock::time_point& time);

		FWindow m_window {WIDTH, HEIGHT, "Frisbee Engine"};
		Device m_device{ m_window };
		Renderer m_renderer{ m_window, m_device };
		RenderSystem m_renderSystem{ m_device, m_renderer.getRenderPass() };

		Input m_input{ m_window };
		Screen m_screen{ m_window };
		Camera m_camera{};

		ModelBuilder m_modelBuilder{ m_device };
		RenderObjectHolder m_renderObjectHolder{};

		std::vector<std::unique_ptr<Script>> m_scripts;
	};
}