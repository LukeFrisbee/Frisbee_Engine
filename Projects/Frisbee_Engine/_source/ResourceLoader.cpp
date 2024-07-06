#include "ResourceLoader.h"

#include "global_data.h"

#include <iostream>
#include <fstream>

namespace fengine {
	ResourceLoader::ResourceLoader(RendererResources& rendererResources)
		: m_rendererResources{ rendererResources }
	{
		std::ofstream ofile("resourceloader.txt");
		if (ofile.is_open()) {
			ofile << "Hellow World!";
		}

		std::string modelFolder = CMAKE_SOURCE_DIR + std::string("\\Projects\\Frisbee_Engine\\models\\");

		Shader::Settings shaderSettings{};

		std::vector<size_t> pbrSizes{ sizeof(uboGlobal), sizeof(uboPBR) };
		auto pbrShaderID = m_rendererResources.createShader(
			"shaders/pbr.vert.spv", "shaders/pbr.frag.spv",
			shaderSettings, pbrSizes);

		std::vector<size_t> albedoSizes{ sizeof(uboGlobal) };
		auto albedoShaderID = m_rendererResources.createShader(
			"shaders/albedo.vert.spv", "shaders/albedo.frag.spv",
			shaderSettings, albedoSizes);

		//std::vector<Model::Vertex> vertices =
		//{
		//	{ { 0.0, 1.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//	{ { 1.0, 1.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//	{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//	{ { 1.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//	{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
		//	{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} }
		//};
		//m_rendererResources.createModel(vertices);
		int jeepId = m_rendererResources.createModel(modelFolder + "jeep.obj");
		int cubeId = m_rendererResources.createModel(modelFolder + "cube.obj");
		int sphereId = m_rendererResources.createModel(modelFolder + "sphere.obj");
		int boardId0 = m_rendererResources.createModel(modelFolder + "board_0.obj");
		int boardId1 = m_rendererResources.createModel(modelFolder + "board_1.obj");

		// JEEP
		{
			auto jeep = RenderObject();
			jeep.modelId = jeepId;
			jeep.shaderId = pbrShaderID;
			jeep.transform.translation = { 5.0f, 0.0f, 0.0f };
			jeep.transform.rotation = { glm::radians(180.0f), 0.0f, 0.0f };

			uboPBR* pbr = new uboPBR;
			jeep.uniforms.push_back(pbr);

			auto id = m_rendererResources.addRenderObject(std::move(jeep));
		}

		// LIGHT CUBE
		{
			auto cube = RenderObject();
			cube.modelId = cubeId;
			//Shader::Settings shaderSettings{};
			cube.shaderId = albedoShaderID;
			cube.transform.translation = { 0.0f, 0.0f, 0.0f };
			GlobalData::getInstance().lightPos = cube.transform.translation;
			cube.transform.scale = { 0.25f, 0.25f, 0.25f };

			auto id = m_rendererResources.addRenderObject(std::move(cube));
		}

		// PBR CUBE
		{
			auto cube = RenderObject();
			cube.modelId = cubeId;
			//Shader::Settings shaderSettings{};
			cube.shaderId = pbrShaderID;
			cube.transform.translation = { 1.0f, -3.0f, -1.0f };
			GlobalData::getInstance().lightPos = cube.transform.translation;
			cube.transform.scale = { 0.5f, 0.5f, 0.5f };

			uboPBR* pbr = new uboPBR;
			pbr->albedo = { 0.1f, 0.4f, 0.1f };
			cube.uniforms.push_back(pbr);

			auto id = m_rendererResources.addRenderObject(std::move(cube));
		}


		// Sphere
		{
			auto sphere = RenderObject();
			sphere.modelId = sphereId;
			sphere.transform.translation = { -5.0f, -0.0f, 0.0f };
			//sphere.transform.scale = { 0.25f, 0.25f, 0.25f };

			Shader::Settings shaderSettings{};
			sphere.shaderId = m_rendererResources.createShader("shaders/pbr.vert.spv", "shaders/pbr.frag.spv", shaderSettings, pbrSizes);

			uboPBR* pbr = new uboPBR;
			pbr->albedo = glm::vec3(1.0f, 1.0f, 1.0f);
			sphere.uniforms.push_back(pbr);

			auto id = m_rendererResources.addRenderObject(std::move(sphere));
		}

		// BOARD
		{
			auto gameObject = RenderObject();
			gameObject.modelId = boardId0;
			gameObject.transform.translation = { 0.0f, 0.0f, 2.0f };
			//sphere.transform.scale = { 0.25f, 0.25f, 0.25f };

			Shader::Settings shaderSettings{};
			gameObject.shaderId = m_rendererResources.createShader("shaders/pbr.vert.spv", "shaders/pbr.frag.spv", shaderSettings, pbrSizes);

			uboPBR* pbr = new uboPBR;
			pbr->albedo = glm::vec3(0.42f, 0.26f, 0.15f);
			pbr->ao = 1.0f;
			gameObject.uniforms.push_back(pbr);

			auto id = m_rendererResources.addRenderObject(std::move(gameObject));
		}
		{
			auto gameObject = RenderObject();
			gameObject.modelId = boardId1;
			gameObject.transform.translation = { 0.0f, 0.0f, 2.0f };
			//sphere.transform.scale = { 0.25f, 0.25f, 0.25f };

			Shader::Settings shaderSettings{};
			gameObject.shaderId = m_rendererResources.createShader("shaders/pbr.vert.spv", "shaders/pbr.frag.spv", shaderSettings, pbrSizes);

			uboPBR* pbr = new uboPBR;
			pbr->albedo = glm::vec3(0.62f, 0.62f, 0.62f);
			pbr->ao = 1.0f;
			gameObject.uniforms.push_back(pbr);

			auto id = m_rendererResources.addRenderObject(std::move(gameObject));
		}
	}
}