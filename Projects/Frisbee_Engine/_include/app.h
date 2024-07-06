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
#include "ResourceLoader.h"
#include "entt/entt.hpp"

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
		void _renderLoop();
		void _time();

		// Vulkan Setup
		FWindow m_window { WIDTH, HEIGHT, "Frisbee Engine" };
		Device m_device{ m_window };

		// Renderer Setup
		Renderer m_renderer{ m_window, m_device };
		RendererResources m_rendererResources{ m_device, m_renderer.getRenderPass() };
		RenderSystem m_renderSystem{ m_device, m_rendererResources };
		Camera m_camera{};

		// Editor
		entt::registry m_ecs;
		EditorUI m_editorUI{ m_window, m_device, m_renderer.getImageCount(), m_renderer.getRenderPass(), m_camera, m_rendererResources, m_ecs };
		
		// Asset Loading
		ResourceLoader m_resourceLoader{ m_rendererResources };
		
		// Script Helpers
		Input m_input{ m_window };
		Screen m_screen{ m_window };
		std::vector<std::unique_ptr<Script>> m_scripts;
	};
}