#include "app.h"

#include <stdexcept>
#include <array>
#include <chrono>

#include "render_system.h"
#include "camera.h"
#include "keyboard_movement.h"
#include "global_data.h"
#include "buffer.h"

namespace fengine {
	App::App() {
		m_renderSystem.createShader();
		loadGameObjects();
	}

	App::~App() {}

	void App::run() 
	{
		// Camera + Movement
		Camera camera{};
		camera.setViewDirection(glm::vec3{}, glm::vec3(0.0f, 0.0f, 1.0f));
		auto cameraObject = GameObject::createGameObject();
		KeyboardMovement cameraMovement{};

		// Timing
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto timeSinceLastSecond = currentTime;
		int fpsCounter = 0;
		auto& globalData = GlobalData::getInstance();
		globalData.fps = 0;

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
			globalData.camPos = cameraObject.transform.translation;
			globalData.camRot = cameraObject.transform.rotation;
			// TODO: Implement on window resize rather than in run loop
			float aspect = m_renderer.getAspectRatio();
			camera.setPerspectiveProject(glm::radians(60.0f), aspect, 0.1f, 100.0f);

			// RENDER
			if (auto commandBuffer = m_renderer.beginFrame()) {

				int frameIndex = m_renderer.getFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera };

				// UBO
				RenderSystem::GlobalUbo ubo{};
				ubo.projectionViewMatrix = camera.getProjection() * camera.getView();
				ubo.camPos = cameraObject.transform.translation;
				ubo.LightPos = m_gameObjects[1].transform.translation;

				//uboBuffers[frameIndex]->writeToBuffer(&ubo);
				//uboBuffers[frameIndex]->flush(); // HOST_COHERENT_BIT is on, flushing automatic

				// RENDER 
				m_renderer.beginRenderPass(commandBuffer);
				m_renderSystem.renderGameObjects(frameInfo, m_gameObjects);
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
		// TEMPORARY -> need to implement the GameObject system

		// JEEP
		std::shared_ptr<Model> model = Model::createModelFromFile(m_device, 
			"C:\\Users\\lukeb\\_Projects\\C++\\Frisbee_Engine\\Frisbee_Engine\\models\\jeep.obj");

		auto jeep = GameObject::createGameObject();
		jeep.model = model;
		jeep.transform.translation = { 0.0f, 0.0f, 0.0f };
		//gameObject.transform.scale = { 2.0f, 2.0f, 2.0f };
		jeep.transform.rotation = { glm::radians(180.0f), 0.0f, 0.0f };
		m_gameObjects.push_back(std::move(jeep));

		// CUBE
		model = Model::createModelFromFile(m_device,
			"C:\\Users\\lukeb\\_Projects\\C++\\Frisbee_Engine\\Frisbee_Engine\\models\\cube.obj");

		auto cube = GameObject::createGameObject();
		cube.model = model;
		cube.transform.translation = { 1.0f, -3.0f, -1.0f };
		cube.transform.scale = { 0.25f, 0.25f, 0.25f };
		//cube.transform.rotation = { glm::radians(180.0f), 0.0f, 0.0f };
		m_gameObjects.push_back(std::move(cube));
	}
}