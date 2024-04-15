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
	}

	void RenderSystem::createShader(const std::string& vertexFilePath, const std::string& fragmentFilePath) {
		m_shaders.push_back(
			std::make_unique<Shader>(m_device, m_renderPass, vertexFilePath, fragmentFilePath)
		);
	}

	void RenderSystem::renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{
		m_shaders[0]->bindPipeline(frameInfo.commandBuffer);

		m_shaders[0]->bindDescriptorSets(frameInfo.commandBuffer, frameInfo.frameIndex);

		for (auto& obj : gameObjects)
		{
			// Push Constants
			SimplePushConstantData push{};
			push.modelMatrix = obj.transform.modelMatrix();
			push.normalMatrix = obj.transform.normalMatrix();

			// UBO
			Shader::GlobalUbo ubo{};
			ubo.projectionViewMatrix = frameInfo.camera.getProjection() * frameInfo.camera.getView();
			ubo.camPos = frameInfo.camera.getPosition();
			ubo.LightPos = gameObjects[1].transform.translation;

			auto& gd = GlobalData::getInstance();
			ubo.albedo = gd.albedo;
			ubo.metallic = gd.metallic;
			ubo.roughness = gd.roughness;
			ubo.ao = gd.ao;

			// Shader Set
			m_shaders[0]->updateDescriptorSets(ubo, frameInfo.frameIndex);
			m_shaders[0]->updatePushConstants(frameInfo.commandBuffer, push);

			// Bind + Draw
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
}