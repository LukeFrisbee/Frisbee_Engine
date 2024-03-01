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
		m_globalDescriptorPool = DescriptorPool::Builder(m_device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	App::~App() {}

	void App::run() 
	{
		// Double BUffering
		std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<Buffer>(
				m_device,
				sizeof(RenderSystem::GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_device.properties.limits.minUniformBufferOffsetAlignment
			);
			uboBuffers[i]->map();
		}

		// Descriptor Sets
		auto globalSetLayout =
			DescriptorSetLayout::Builder(m_device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *m_globalDescriptorPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		// Render Sysem
		RenderSystem renderSystem { m_device, m_renderer.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

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
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

				// UBO
				RenderSystem::GlobalUbo ubo{};
				ubo.projectionViewMatrix = camera.getProjection() * camera.getView();
				ubo.camPos = cameraObject.transform.translation;
				ubo.LightPos = m_gameObjects[1].transform.translation;

				ubo.albedo = globalData.albedo;
				ubo.roughness = globalData.roughness;
				ubo.metallic = globalData.metallic;
				ubo.ao = globalData.ao;

				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				//uboBuffers[frameIndex]->flush(); // HOST_COHERENT_BIT is on, flushing automatic

				// RENDER 
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