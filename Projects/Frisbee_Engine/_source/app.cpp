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
#include "rope.h"
#include "test_script.h"

#include "rope_connector.h"
#include "camera_movement.h"

namespace fengine {
	App::App() {
		_loadGameObjects();

		//TestScript script{};
		m_scripts.push_back(std::make_unique<CameraMovement>(m_input, m_screen, m_camera, m_renderObjectHolder, m_modelBuilder));
		m_scripts.push_back(std::make_unique<RopeConnector>(m_input, m_screen, m_camera, m_renderObjectHolder, m_modelBuilder));
	}

	void App::run() 
	{
		auto& globalData = GlobalData::getInstance();
		globalData.fps = 0;

		// Timing
		auto lastTime = std::chrono::high_resolution_clock::now();

		m_camera.setViewDirection(glm::vec3{}, glm::vec3(0.0f, 0.0f, 0.5f));
		RenderObject cameraObject = RenderObject();

		for(auto& script : m_scripts) {
			script->Start();
		}

		while (!m_window.shouldClose()) {
			glfwPollEvents();

			_time(lastTime);

			for (auto& script : m_scripts) {
				script->Update();
			}

			_renderLoop(cameraObject);

		}

		// Stops errors from being thrown if GPU is still processing command buffers
		// (Block CPU until all GPU operations have completed)
		vkDeviceWaitIdle(m_device.device());
	}

	void App::_time(std::chrono::steady_clock::time_point& lastTime)
	{
		static int fpsCounter = 0;
		auto& globalData = GlobalData::getInstance();

		// Loop Timing
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>
			(newTime - lastTime).count();
		LoopTime::setDeltaTime(frameTime);

		// FpsCounter
		auto diff = newTime - lastTime;
		if (std::chrono::duration_cast<std::chrono::seconds>(diff).count() >= 1)
		{
			globalData.fps = fpsCounter;
			fpsCounter = 0;
			lastTime = std::chrono::steady_clock::now();
		}
		fpsCounter++;

		lastTime = newTime;
	}

	void App::_renderLoop( RenderObject& cameraObject)
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

			m_renderSystem.renderGameObjects(frameInfo, m_renderObjectHolder.getRenderObjects());

			m_renderer.endRenderPass(commandBuffer);
			m_renderer.endFrame();
		}
	}

	void App::_loadGameObjects()
	{
		// TEMPORARY -> need to implement the RenderObject system

		std::string modelFolder = CMAKE_SOURCE_DIR + std::string("\\Projects\\Frisbee_Engine\\models\\");

		Shader::Settings shaderSettings{};

		std::vector<size_t> pbrSizes { sizeof(uboGlobal), sizeof(uboPBR) };
		auto pbrShaderID = m_renderSystem.createShader(
			"shaders/pbr.vert.spv", "shaders/pbr.frag.spv", 
			shaderSettings, pbrSizes);

		std::vector<size_t> albedoSizes{ sizeof(uboGlobal) };
		auto albedoShaderID = m_renderSystem.createShader(
			"shaders/albedo.vert.spv", "shaders/albedo.frag.spv",
			shaderSettings, albedoSizes);

		// JEEP
		{
			auto jeep = RenderObject();
			jeep.model = m_modelBuilder.BuildModelFromFile(modelFolder + "jeep.obj");
			jeep.shaderId = pbrShaderID;
			jeep.transform.translation = { 5.0f, 0.0f, 0.0f };
			jeep.transform.rotation = { glm::radians(180.0f), 0.0f, 0.0f };
		
			uboPBR* pbr = new uboPBR;
			jeep.uniforms.push_back(pbr);
		
			m_renderObjectHolder.addRenderObject(std::move(jeep));
		}
		//
		// LIGHT CUBE
		{
			auto cube = RenderObject();
			std::shared_ptr<Model> model = m_modelBuilder.BuildModelFromFile(modelFolder + "cube.obj");
			cube.model = model;
			Shader::Settings shaderSettings{};
			cube.shaderId = albedoShaderID;
			cube.transform.translation = { 1.0f, -3.0f, -1.0f };
			GlobalData::getInstance().lightPos = cube.transform.translation;
			cube.transform.scale = { 0.25f, 0.25f, 0.25f };
		
			m_renderObjectHolder.addRenderObject(std::move(cube));
		}
		
		// Sphere
		{
			auto sphere = RenderObject();
			std::shared_ptr<Model> model = m_modelBuilder.BuildModelFromFile(modelFolder + "sphere.obj");
			sphere.model = model;
			sphere.transform.translation = { -5.0f, -0.0f, 0.0f };
			//sphere.transform.scale = { 0.25f, 0.25f, 0.25f };
		
			Shader::Settings shaderSettings{};
			sphere.shaderId = m_renderSystem.createShader( "shaders/pbr.vert.spv", "shaders/pbr.frag.spv", shaderSettings, pbrSizes);
			
			uboPBR* pbr = new uboPBR;
			pbr->albedo = glm::vec3(1.0f, 1.0f, 1.0f);
			sphere.uniforms.push_back(pbr);
		
			m_renderObjectHolder.addRenderObject(std::move(sphere));
		}
		
		// Sphere
		{
			auto sphere = RenderObject();
			std::shared_ptr<Model> model = m_modelBuilder.BuildModelFromFile(modelFolder + "sphere.obj");
			sphere.model = model;
			sphere.transform.translation = { -5.0f, -0.0f, 0.0f };
			sphere.transform.scale = { 0.25f, 0.25f, 0.25f };
		
			Shader::Settings shaderSettings{};
			sphere.shaderId = m_renderSystem.createShader("shaders/pbr.vert.spv", "shaders/pbr.frag.spv", shaderSettings, pbrSizes);
		
			uboPBR* pbr = new uboPBR;
			pbr->albedo = glm::vec3(1.0f, 0.0f, 0.0f);
			sphere.uniforms.push_back(pbr);
		
			m_renderObjectHolder.addRenderObject(std::move(sphere));
		}
		
		// BOARD
		{
			auto gameObject = RenderObject();
			std::shared_ptr<Model> model = m_modelBuilder.BuildModelFromFile(modelFolder + "board_0.obj");
			gameObject.model = model;
			gameObject.transform.translation = { 0.0f, 0.0f, 2.0f };
			//sphere.transform.scale = { 0.25f, 0.25f, 0.25f };
		
			Shader::Settings shaderSettings{};
			gameObject.shaderId = m_renderSystem.createShader("shaders/pbr.vert.spv", "shaders/pbr.frag.spv", shaderSettings, pbrSizes);
		
			uboPBR* pbr = new uboPBR;
			pbr->albedo = glm::vec3(0.42f, 0.26f, 0.15f);
			pbr->ao = 1.0f;
			gameObject.uniforms.push_back(pbr);
		
			m_renderObjectHolder.addRenderObject(std::move(gameObject));
		}
		{
			auto gameObject = RenderObject();
			std::shared_ptr<Model> model = m_modelBuilder.BuildModelFromFile(modelFolder + "board_1.obj");
			gameObject.model = model;
			gameObject.transform.translation = { 0.0f, 0.0f, 2.0f };
			//sphere.transform.scale = { 0.25f, 0.25f, 0.25f };
		
			Shader::Settings shaderSettings{};
			gameObject.shaderId = m_renderSystem.createShader("shaders/pbr.vert.spv", "shaders/pbr.frag.spv", shaderSettings, pbrSizes);
		
			uboPBR* pbr = new uboPBR;
			pbr->albedo = glm::vec3(0.62f, 0.62f, 0.62f);
			pbr->ao = 1.0f;
			gameObject.uniforms.push_back(pbr);
		
			m_renderObjectHolder.addRenderObject(std::move(gameObject));
		}

		//{ // ROPE
		//	RenderObject rope = RenderObject();
		//	Model::Data data{};
		//	data.vertices =
		//	{
		//		{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//		{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//		{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//		{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//		{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//		{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} }
		//	};
		//	rope.model = m_modelBuilder.BuildModel(data);
		//	
		//	std::vector<size_t> albedoSizes{ sizeof(uboGlobal) };
		//	Shader::Settings shaderSettings{};
		//	rope.shaderId = m_renderSystem.createShader("shaders/albedo.vert.spv", "shaders/albedo.frag.spv", shaderSettings, albedoSizes);
		//}
	}
}