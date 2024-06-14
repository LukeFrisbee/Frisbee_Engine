#include "shader.h"
#include <memory>
#include <cassert>
#include <stdexcept>
#include <glm/glm.hpp>

#include "swap_chain.h"
#include "descriptors.h"

#include "global_data.h"

#include <iostream>

#include "VmaUsage.h"

namespace fengine {
	Shader::Shader(Device& device, 
		const VkRenderPass& renderPass, 
		const Shader::Settings& settings, 
		const std::vector<size_t>& uboSizes,
		const std::string& vertexPath, 
		const std::string& fragPath)
		: m_device{ device }
	{
		_buildDescriptorSet(uboSizes);
		_createPipelineLayout();
		_createPipeline(renderPass, settings, vertexPath, fragPath);
	}

	Shader::~Shader()
	{
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);

		for (auto& uniform : m_uniforms)
		{
			for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
				vmaDestroyBuffer(m_device.vmaAllocator(), uniform.buffers[i], uniform.allocations[i]);
			}
		}
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

	void Shader::updateDescriptorSets(UBO& ubo, int binding, int frameIndex)
	{
		assert(frameIndex <= SwapChain::MAX_FRAMES_IN_FLIGHT && "shader: frameIndex out of bounds (Max Frames In Flight)");
		assert(binding < m_uniforms.size() && "shader: binding out of bounds");

		memcpy(m_uniforms[binding].allocationInfos[frameIndex].pMappedData, 
			&ubo, 
			m_uniforms[binding].allocationInfos[frameIndex].size);
	}

	void Shader::_buildDescriptorSet(const std::vector<size_t>& uboSizes)
	{
		assert(uboSizes.size() > 0 && "shader: _buildDescriptorSet uboSizes cannot be empty!");

		m_uniforms = std::vector<VMABuffer>(uboSizes.size());
		for (int binding = 0; binding < uboSizes.size(); binding++) {
			for (int frameIndex = 0; frameIndex < SwapChain::MAX_FRAMES_IN_FLIGHT; frameIndex++) {
				// Staging Buffer
				VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
				bufferCreateInfo.size = uboSizes[binding];
				bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				VmaAllocationCreateInfo allocCreateInfo = {};
				allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
				allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

				vmaCreateBuffer(m_device.vmaAllocator(), &bufferCreateInfo, &allocCreateInfo,
					&m_uniforms[binding].buffers[frameIndex],
					&m_uniforms[binding].allocations[frameIndex],
					&m_uniforms[binding].allocationInfos[frameIndex]);
			}
		}

		m_globalDescriptorPool = DescriptorPool::Builder(m_device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
			.build();

		// Descriptor Sets
		auto temp = DescriptorSetLayout::Builder(m_device);
		for (int binding = 0; binding < m_uniforms.size(); binding++) {
			temp.addBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		}
		m_globalSetLayout = temp.build();

		std::vector<VkDescriptorBufferInfo> bufferInfos = std::vector<VkDescriptorBufferInfo>(m_uniforms.size());

		for (int frameIndex = 0; frameIndex < SwapChain::MAX_FRAMES_IN_FLIGHT; frameIndex++) {

			auto writer = DescriptorWriter(*m_globalSetLayout, *m_globalDescriptorPool);
			
			for (int binding = 0; binding < m_uniforms.size(); binding++) {
				bufferInfos[binding].buffer = m_uniforms[binding].buffers[frameIndex];
				bufferInfos[binding].offset = 0;
				bufferInfos[binding].range = VK_WHOLE_SIZE;

				writer.writeBuffer(binding, &bufferInfos[binding]);
			}

			writer.build(m_descriptorSets[frameIndex]);
		}

		m_descriptorSetLayouts.push_back(m_globalSetLayout->getDescriptorSetLayout());
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

	void Shader::_createPipeline(const VkRenderPass& renderPass, const Shader::Settings& settings, const std::string& vertexPath, const std::string& fragPath) {
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		FPipeline::defaultPipeLineConfigInfo(
			pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline = std::make_unique<FPipeline>(
			m_device,
			pipelineConfig,
			vertexPath,
			fragPath);
	}
}

