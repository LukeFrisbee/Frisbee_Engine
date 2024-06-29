#include "app.h"

#include <stdexcept>
#include <array>
#include <iostream>

#include "render_system.h"
#include "camera.h"
#include "global_data.h"

#include "physics/physics_sphere.h"
#include "physics/ray_cast.h"
#include "loop_time.h"

#include "board.h"
#include "test_script.h"

#include "rope_connector.h"
#include "camera_movement.h"

#include "logger.h"

namespace fengine {
	App::App() {
		//TestScript script{};

		Logger::Init(m_editorUI);
		LOG_WARN("test!");

		m_scripts.push_back(std::make_unique<CameraMovement>(m_camera, m_input, m_screen));
		//m_scripts.push_back(std::make_unique<RopeConnector>(m_camera, m_input, m_screen, m_modelBuilder, m_rendererResources));
	}

	void App::run()
	{
		auto& globalData = GlobalData::getInstance();
		globalData.fps = 0;

		m_camera.setPosition(glm::vec3(-5.0f, -1.0f, 0.0f));
		//m_camera.setViewDirection(glm::vec3{}, glm::vec3(0.0f, 0.0f, 0.5f));

		for (auto& script : m_scripts) {
			script->Start();
		}

		while (!m_window.shouldClose()) {
			glfwPollEvents();

			for (auto& script : m_scripts) {
				script->Update();
			}
			_time();
			
			_renderLoop();

		}

		// Stops errors from being thrown if GPU is still processing command buffers
		// (Block CPU until all GPU operations have completed)
		vkDeviceWaitIdle(m_device.device());
	}

	void App::_time()
	{
		// Timing
		static auto lastFrameTime = std::chrono::high_resolution_clock::now();
		static auto lastSecondTimePoint = std::chrono::high_resolution_clock::now();

		static int fpsCounter = 0;
		auto& globalData = GlobalData::getInstance();

		// Loop Timing
		auto currentTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - lastFrameTime).count();

		LoopTime::setDeltaTime(frameTime);

		// FpsCounter
		auto diff = currentTime - lastSecondTimePoint;
		if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() >= 1000)
		{
			globalData.fpsQueue.push(fpsCounter);
			if (globalData.fpsQueue.size() > 10)
				globalData.fpsQueue.pop();

			LOG_DEBUG(fpsCounter);
			globalData.fps = fpsCounter;
			fpsCounter = 0;
			lastSecondTimePoint = currentTime;
		}

		lastFrameTime = currentTime;
		fpsCounter++;
	}

	void App::_renderLoop()
	{
		float frameTime = LoopTime::getDeltaTime();
		auto& globalData = GlobalData::getInstance();

		// TODO: Implement on window resize rather than in run loop
		float aspect = m_renderer.getAspectRatio();
		m_camera.setPerspectiveProject(glm::radians(60.0f), aspect, 0.1f, 100.0f);

		if (auto commandBuffer = m_renderer.beginFrame()) {

			int frameIndex = m_renderer.getFrameIndex();
			FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, m_camera };

			// RENDER 
			m_renderer.beginRenderPass(commandBuffer);

			m_renderSystem.renderGameObjects(frameInfo);
			m_editorUI.DrawFrame(commandBuffer);

			m_renderer.endRenderPass(commandBuffer);
			m_renderer.endFrame();
		}
	}
}