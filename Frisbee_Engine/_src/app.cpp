#include "app.h"

#include <stdexcept>
#include <array>
#include <chrono>

#include "render_system.h"
#include "camera.h"
#include "keyboard_movement.h"
#include "global_data.h"
#include "buffer.h"

struct GlobalUbo {
	glm::mat4 projectionView{ 1.0f };
	glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -3.0f, -1.0f));
};

namespace fengine {
	App::App() {
		loadGameObjects();
	}

	App::~App() {}

	void App::run() 
	{
		// Double BUffering
		std::vector<std::unique_ptr<Buffer>> uniformBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uniformBuffers.size(); i++)
		{
			uniformBuffers[i] = std::make_unique<Buffer>(
				m_device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_device.properties.limits.minUniformBufferOffsetAlignment
			);
			uniformBuffers[i]->map();
		}

		RenderSystem renderSystem{m_device, m_renderer.getRenderPass() };
		Camera camera{};
		camera.setViewDirection(glm::vec3{}, glm::vec3(0.0f, 0.0f, 1.0f));

		auto currentTime = std::chrono::high_resolution_clock::now();
		auto timeSinceLastSecond = currentTime;

		auto cameraObject = GameObject::createGameObject();
		KeyboardMovement cameraMovement{};

		auto& globalData = GlobalData::getInstance();
		globalData.fps = 0;

		int fpsCounter = 0;
		while (!m_window.shouldClose()) {
			glfwPollEvents();

			// Loop Timing
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>
				(newTime - currentTime).count();
			currentTime = newTime;

			auto diff = newTime - timeSinceLastSecond;
			if (std::chrono::duration_cast<std::chrono::seconds>(diff).count() >= 1)
			{
				globalData.fps = fpsCounter;
				fpsCounter = 0;
				timeSinceLastSecond = std::chrono::steady_clock::now();
			}

			// Camera
			cameraMovement.moveInPlaneXZ(m_window.getGLFWwindow(), frameTime, cameraObject);
			camera.setViewYXZ(cameraObject.transform.translation, cameraObject.transform.rotation);
			// TODO: Implement on window resize rather than in run loop
			float aspect = m_renderer.getAspectRatio();
			camera.setPerspectiveProject(glm::radians(60.0f), aspect, 0.1f, 100.0f);

			// RENDER LOOP
			if (auto commandBuffer = m_renderer.beginFrame()) {
				int frameIndex = m_renderer.getFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera };

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				//uniformBuffers[frameIndex]->flush(); // HOST_COHERENT_BIT is on, flushing automatic

				// render (draw calls)
				m_renderer.beginRenderPass(commandBuffer);
				renderSystem.renderGameObjects(frameInfo, m_gameObjects);
				m_renderer.endRenderPass(commandBuffer);
				m_renderer.endFrame();
			}
			fpsCounter++;
		}

		// Stops errors from being thrown if GPU is still processing command buffers
		// (Block CPU until all GPU operations have completed)
		vkDeviceWaitIdle(m_device.device());
	}

	void App::loadGameObjects()
	{
		std::shared_ptr<Model> model = Model::createModelFromFile(m_device, "C:\\Users\\lukeb\\_Projects\\C++\\Frisbee_Engine\\Frisbee_Engine\\models\\flat_vase.obj");
		//auto Model = std::make_shared<Model>(m_device, vertices);

		auto gameObject = GameObject::createGameObject();
		gameObject.model = model;
		gameObject.transform.translation = { 0.0f, 0.0f, 1.0f };
		gameObject.transform.scale = { 2.0f, 2.0f, 2.0f };
		//gameObject.Transform.rotation = { glm::radians(180.0f), 0.0f, 0.0f };
		m_gameObjects.push_back(std::move(gameObject));
	}
}