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
	RenderSystem::RenderSystem(Device& device, RendererResources& resources)
		: m_device{ device }, m_resources{ resources }
	{
		Shader::Settings settings{};
		settings.Wireframe = true;
		//createShader("shaders/pbr.vert.spv", "shaders/pbr.frag.spv", settings);
	}

	void RenderSystem::renderGameObjects(const FrameInfo& frameInfo) const
	{
		auto& gd = GlobalData::getInstance();

		uboGlobal ubo{};
		ubo.projectionViewMatrix = frameInfo.camera.getProjection() * frameInfo.camera.getView();
		ubo.camPos = frameInfo.camera.getPosition();
		ubo.lightPos = gd.lightPos;

		for (auto& obj : m_resources.m_renderObjects)
		{
			// TODO: This should be handeled per shader, not per gameobject (Material Instancing)
			m_resources.m_shaders[obj.shaderId]->bindDescriptorSets(frameInfo.commandBuffer, frameInfo.frameIndex);
			m_resources.m_shaders[obj.shaderId]->bindPipeline(frameInfo.commandBuffer);

			// Push Constants
			SimplePushConstantData push{};
			push.modelMatrix = obj.transform.modelMatrix();
			push.normalMatrix = obj.transform.normalMatrix();
			m_resources.m_shaders[obj.shaderId]->updatePushConstants(frameInfo.commandBuffer, push);

			// Desctriper Sets
			m_resources.m_shaders[obj.shaderId]->updateDescriptorSets(ubo, 0, frameInfo.frameIndex);

			for (int i = 0; i < obj.uniforms.size(); i++) {
				m_resources.m_shaders[obj.shaderId]->updateDescriptorSets(*obj.uniforms[i], i+1, frameInfo.frameIndex);
			}
			
			// Bind + Draw
			m_resources.m_models[obj.modelId]->bind(frameInfo.commandBuffer);
			m_resources.m_models[obj.modelId]->draw(frameInfo.commandBuffer);
		}
	}
}