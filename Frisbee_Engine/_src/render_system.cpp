#include "render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#include <stdexcept>
#include <array>

#include "global_data.h"

#include "swap_chain.h"
#include "descriptors.h"

namespace fengine {
	RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass)
		: m_device { device } , m_renderPass { renderPass }
	{
		//_makeWireframe(renderPass);
	}

	//void RenderSystem::_makeWireframe(const VkRenderPass& renderPass, const VkDescriptorSetLayout& descriptorSetLayout)
	//{
	//	VkPushConstantRange pushConstantRange{};
	//	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	//	pushConstantRange.offset = 0;
	//	pushConstantRange.size = sizeof(SimplePushConstantData);
	//
	//	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ descriptorSetLayout };
	//
	//	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	//	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	//	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	//	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	//	pipelineLayoutInfo.pushConstantRangeCount = 1;
	//	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	//
	//	VkPipelineLayout pipelineLayout;
	//	if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("Failed to create pipeline layout!");
	//	}
	//
	//	PipelineConfigInfo pipelineConfig{};
	//	FPipeline::defaultPipeLineConfigInfo(
	//		pipelineConfig);
	//
	//	pipelineConfig.renderPass = renderPass;
	//	pipelineConfig.pipelineLayout = pipelineLayout;
	//	pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE; // Wireframe
	//
	//	m_wireframePipeline = std::make_unique<FPipeline>(
	//		m_device,
	//		pipelineConfig,
	//		"shaders/white.vert.spv",
	//		"shaders/white.frag.spv");
	//}

	void RenderSystem::createShader() {
		m_shaders.push_back(
			std::make_unique<Shader>(m_device, m_renderPass, "shaders/pbr.vert.spv", "shaders/pbr.frag.spv")
		);
	}

	void RenderSystem::renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{
		if (GlobalData::getInstance().isWireFrame)
			m_wireframePipeline->bind(frameInfo.commandBuffer);
		else
			m_shaders[0]->bindPipeline(frameInfo.commandBuffer);

		m_shaders[0]->bindDescriptorSets(frameInfo.commandBuffer, frameInfo.frameIndex);

		for (auto& obj : gameObjects)
		{
			SimplePushConstantData push{};
			push.modelMatrix = obj.transform.modelMatrix();
			push.normalMatrix = obj.transform.normalMatrix();

			m_shaders[0]->updatePushConstants(frameInfo.commandBuffer, push);

			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
}