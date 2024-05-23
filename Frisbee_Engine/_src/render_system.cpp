#include "render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#include <stdexcept>
#include <array>

#include "global_data.h"

#include "shader.h"

namespace fengine {
	RenderSystem::RenderSystem(Device& device, const VkRenderPass& renderPass)
		: m_device { device } , m_renderPass { renderPass }
	{
		Shader::Settings settings{};
		settings.Wireframe = true;
		//createShader("shaders/pbr.vert.spv", "shaders/pbr.frag.spv", settings);
	}

	int RenderSystem::createShader(const std::string& vertexFilePath, const std::string& fragmentFilePath, const Shader::Settings& settings) {
		m_shaders.push_back(
			std::make_unique<Shader>(m_device, m_renderPass, settings, vertexFilePath, fragmentFilePath)
		);

		static unsigned int shaderId = 0;
		shaderId++;
		return shaderId - 1;
	}

	void RenderSystem::renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{
		for (auto& obj : gameObjects)
		{
			// TODO: This should be handeled per shader, not per gameobject (Material Instancing)
			m_shaders[obj.shaderId]->bindDescriptorSets(frameInfo.commandBuffer, frameInfo.frameIndex);
			m_shaders[obj.shaderId]->bindPipeline(frameInfo.commandBuffer);

			// Push Constants
			SimplePushConstantData push{};
			push.modelMatrix = obj.transform.modelMatrix();
			push.normalMatrix = obj.transform.normalMatrix();
			m_shaders[obj.shaderId]->updatePushConstants(frameInfo.commandBuffer, push);

			// Desctriper Sets
			uboGlobal ubo{};
			ubo.projectionViewMatrix = frameInfo.camera.getProjection() * frameInfo.camera.getView();
			ubo.camPos = frameInfo.camera.getPosition();
			ubo.lightPos = gameObjects[1].transform.translation;
			m_shaders[obj.shaderId]->updateDescriptorSets(ubo, 0, frameInfo.frameIndex);

			//auto& gd = GlobalData::getInstance();
			//uboPBR pbr{};
			//pbr.albedo = gd.albedo;
			//pbr.metallic = gd.metallic;
			//pbr.roughness = gd.roughness;
			//pbr.ao = gd.ao;
			//m_shaders[obj.shaderId]->updateDescriptorSets(pbr, 1, frameInfo.frameIndex);

			// Bind + Draw
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
}