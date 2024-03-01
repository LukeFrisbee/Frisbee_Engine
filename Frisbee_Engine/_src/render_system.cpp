#include "render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#include <stdexcept>
#include <array>

#include "global_data.h"

namespace fengine {
	RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
		: m_device { device } 
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem() {
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void RenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		FPipeline::defaultPipeLineConfigInfo(
			pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		// TODO: Pipeline derivative or Cache (Instead of rebuilding a pipeline from scratch)
		// PBR
		m_fPipelinePBR = std::make_unique<FPipeline>(
			m_device,
			pipelineConfig,
			"shaders/pbr.vert.spv",
			"shaders/pbr.frag.spv");

		// WHITE
		m_fPipelineWhite = std::make_unique<FPipeline>(
			m_device,
			pipelineConfig,
			"shaders/white.vert.spv",
			"shaders/white.frag.spv");

		// WIREFRAME
		pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
		m_fPipelineWireFrame = std::make_unique<FPipeline>(
			m_device,
			pipelineConfig,
			"shaders/white.vert.spv",
			"shaders/white.frag.spv");
	}

	void RenderSystem::renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{
		// WIREFRAME
		if (GlobalData::getInstance().isWireFrame)
			m_fPipelineWireFrame->bind(frameInfo.commandBuffer);
		else
			m_fPipelinePBR->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		int i = 0;
		for (auto& obj : gameObjects)
		{
			if (!GlobalData::getInstance().isWireFrame && i > 0)
				m_fPipelineWhite->bind(frameInfo.commandBuffer);
			i++;
			//auto uiInputData = m_draw->GetUIInputData();

			SimplePushConstantData push{};
			push.modelMatrix = obj.transform.modelMatrix();
			push.normalMatrix = obj.transform.normalMatrix();

			//push.offset = uiInputData.position; //obj.transform2D.translation;
			//push.Color = obj.Color;

			vkCmdPushConstants(frameInfo.commandBuffer, m_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0, sizeof(SimplePushConstantData), &push);

			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
}