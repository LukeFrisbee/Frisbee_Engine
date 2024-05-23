#include "app.h"

#include <stdexcept>
#include <array>
#include <chrono>

#include "render_system.h"
#include "camera.h"
#include "keyboard_movement.h"
#include "fps_control.h"
#include "global_data.h"
#include "buffer.h"

#include "physics/physics_sphere.h"; 
#include "physics/ray_cast.h"

#include <iostream>

namespace fengine {
	App::App() {
		loadGameObjects();
	}

	App::~App() {}

	void App::run() 
	{
		// Camera + Movement
		Camera camera{};
		camera.setViewDirection(glm::vec3{}, glm::vec3(0.0f, 0.0f, 1.0f));
		auto cameraObject = GameObject::createGameObject();
		//KeyboardMovement cameraMovement{};
		FPSControl fpsControl{};

		// Timing
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto timeSinceLastSecond = currentTime;
		int fpsCounter = 0;
		auto& globalData = GlobalData::getInstance();
		globalData.fps = 0;

		PhysicsSphere physicsSphere{ glm::vec3{0.0, 0.0, 0.0}, 1.5 };

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

			RayCast raycast{};
			PhysicsShape* shape = nullptr;
			if (raycast.castLine(shape, globalData.lineStart, globalData.lineEnd))
			{
				globalData.debugString += "found shape!\n";
			}

			// Camera
			//cameraMovement.moveInPlaneXZ(m_window.getGLFWwindow(), frameTime, cameraObject);
			fpsControl.moveInPlaneXZ(m_window, frameTime, cameraObject);
			camera.setViewYXZ(cameraObject.transform.translation, cameraObject.transform.rotation);
			camera.setPosition(cameraObject.transform.translation);

			globalData.camPos = cameraObject.transform.translation;
			globalData.camRot = cameraObject.transform.rotation;

			// TODO: Implement on window resize rather than in run loop
			float aspect = m_renderer.getAspectRatio();
			camera.setPerspectiveProject(glm::radians(60.0f), aspect, 0.1f, 100.0f);

			// RENDER
			if (auto commandBuffer = m_renderer.beginFrame()) {

				int frameIndex = m_renderer.getFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera };

				// RENDER 
				m_renderer.beginRenderPass(commandBuffer);

				m_renderSystem.renderGameObjects(frameInfo, m_gameObjects);

				m_renderer.endRenderPass(commandBuffer);
				m_renderer.endFrame();
			}

			auto isColliding = physicsSphere.collisionAtPos(cameraObject.transform.translation);
			globalData.debugString += std::to_string(isColliding) + '\n';

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
		{
			auto jeep = GameObject::createGameObject();
			std::shared_ptr<Model> model = Model::createModelFromFile(m_device,
				"C:\\Users\\lukeb\\_Projects\\C++\\Frisbee_Engine\\Frisbee_Engine\\models\\jeep.obj");
			jeep.model = model;	
			Shader::Settings shaderSettings{};
			jeep.shaderId = m_renderSystem.createShader("shaders/albedo.vert.spv", "shaders/albedo.frag.spv", shaderSettings);
			jeep.transform.translation = { 0.0f, 0.0f, 0.0f };
			jeep.transform.rotation = { glm::radians(180.0f), 0.0f, 0.0f };
			m_gameObjects.push_back(std::move(jeep));
		}

		// CUBE
		{
			auto cube = GameObject::createGameObject();
			std::shared_ptr<Model> model = Model::createModelFromFile(m_device,
				"C:\\Users\\lukeb\\_Projects\\C++\\Frisbee_Engine\\Frisbee_Engine\\models\\cube.obj");
			cube.model = model;
			Shader::Settings shaderSettings{};
			cube.shaderId = m_renderSystem.createShader("shaders/albedo.vert.spv", "shaders/albedo.frag.spv", shaderSettings);
			cube.transform.translation = { 1.0f, -3.0f, -1.0f };
			cube.transform.scale = { 0.25f, 0.25f, 0.25f };
			m_gameObjects.push_back(std::move(cube));
		}
	}
}