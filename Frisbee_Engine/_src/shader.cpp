#include "shader.h"
#include <memory>
#include <cassert>
#include <stdexcept>
#include <glm/glm.hpp>

#include "swap_chain.h"
#include "descriptors.h"
#include "buffer.h"
#include "render_system.h"

namespace fengine {
	Shader::Shader(Device& device, const VkRenderPass& renderPass, const char* vertexPath, const char* fragPath)
		: m_device{ device }
	{
		m_globalDescriptorPool = DescriptorPool::Builder(m_device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		_buildDescriptorSet();
		_createPipelineLayout();
		_createPipeline(renderPass);
	}

	Shader::~Shader()
	{
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void Shader::_buildDescriptorSet()
	{
		for (int i = 0; i < m_uboBuffers.size(); i++)
		{
			m_uboBuffers[i] = std::make_unique<Buffer>(
				m_device,
				sizeof(RenderSystem::GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_device.properties.limits.minUniformBufferOffsetAlignment
			);
			m_uboBuffers[i]->map();
		}

		// Descriptor Sets
		auto globalSetLayout =
			DescriptorSetLayout::Builder(m_device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		//std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_descriptorSets.size(); i++) {
			auto bufferInfo = m_uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *m_globalDescriptorPool)
				.writeBuffer(0, &bufferInfo)
				.build(m_descriptorSets[i]);
		}

		m_descriptorSetLayouts.push_back(globalSetLayout->getDescriptorSetLayout());
	}

	void Shader::bindPipeline(const VkCommandBuffer& commandBuffer) {
		m_pipeline->bind(commandBuffer);
	}

	void Shader::bindDescriptorSets(const VkCommandBuffer& commandBuffer, int frameIndex)
	{
		assert(frameIndex <= SwapChain::MAX_FRAMES_IN_FLIGHT && "shader: frameIndex out of bounds (Max Frames In Flight)");

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			1,
			&m_descriptorSets[frameIndex],
			0,
			nullptr);
	}
	
	void Shader::updatePushConstants(const VkCommandBuffer& commandBuffer, const SimplePushConstantData& push)
	{
		vkCmdPushConstants(commandBuffer, m_pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0, sizeof(SimplePushConstantData), &push);
	}

	void Shader::_createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = m_descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void Shader::_createPipeline(const VkRenderPass& renderPass) {
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		FPipeline::defaultPipeLineConfigInfo(
			pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline = std::make_unique<FPipeline>(
			m_device,
			pipelineConfig,
			"shaders/pbr.vert.spv",
			"shaders/pbr.frag.spv");
	}
}

