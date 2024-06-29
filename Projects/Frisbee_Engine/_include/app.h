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
#include "ResourceLoader.h"

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

		FWindow m_window { WIDTH, HEIGHT, "Frisbee Engine" };
		Device m_device{ m_window };
		Renderer m_renderer{ m_window, m_device };

		RendererResources m_rendererResources{ m_device, m_renderer.getRenderPass() };
		ModelBuilder m_modelBuilder{ m_device };

		RenderSystem m_renderSystem{ m_device, m_rendererResources };
		EditorUI m_editorUI{ m_window, m_device, m_renderer.getImageCount(), m_renderer.getRenderPass(), m_camera, m_rendererResources };
		ResourceLoader m_resourceLoader{ m_rendererResources, m_modelBuilder, m_editorUI };

		// Script Helpers
		Input m_input{ m_window };
		Screen m_screen{ m_window };
		Camera m_camera{};


		std::vector<std::unique_ptr<Script>> m_scripts;
	};
}