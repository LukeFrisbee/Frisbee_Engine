#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "pipeline.h"
#include "swap_chain.h"
#include "descriptors.h"

namespace fengine {
	struct UBO {
		// Alightment Requirements for UBO:
		// Multiples of 16
		// https://vulkan-tutorial.com/Uniform_buffers/Descriptor_pool_and_sets#page_Alignment-requirements
	};

	struct uboGlobal : public UBO {
		glm::mat4 projectionViewMatrix{ 1.0f };
		glm::vec3 lightPos = glm::vec3(1.0f, -4.0f, -1.0f);
		alignas(16) glm::vec3 camPos = {};
		//float metallic{ 0.0f };
		//float roughness{ 0.3f };
	};

	struct uboPBR : public UBO {
		glm::vec3 albedo{ 1.0f, 0.0f, 0.0f };
		float metallic{ 0.0f };
		float roughness{ 0.3f };
		float ao{ 0.02f };
	};

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
		//alignas(16) glm::vec3 Color{};
	};

	class Shader {
	public:
		struct Settings {
			bool Wireframe{ false };
		};

		Shader(Device& device, const VkRenderPass& renderPass, const Shader::Settings& settings, const std::string& vertexPath, const std::string& fragPath);
		~Shader();

		void bindPipeline(const VkCommandBuffer& commandBuffer);
		void bindDescriptorSets(const VkCommandBuffer& commandBuffer, int frameIndex);
		void updatePushConstants(const VkCommandBuffer& commandBuffer, const SimplePushConstantData& push);
		void updateDescriptorSets(UBO& ubo, int binding, int frameIndex);

		//Shader(const Shader&) = delete;
		//Shader& operator= (const Shader&) = delete;

	private:
		void _buildDescriptorSet();
		void _createPipelineLayout();
		void _createPipeline(const VkRenderPass& renderPass, const Shader::Settings& settings, const std::string& vertexPath, const std::string& fragPath);

		Device& m_device;
		VkPipelineLayout m_pipelineLayout;
		std::unique_ptr<FPipeline> m_pipeline;

		struct VMABuffer {
			std::vector<VkBuffer> buffers = std::vector<VkBuffer>(SwapChain::MAX_FRAMES_IN_FLIGHT);
			std::vector<VmaAllocation> allocations = std::vector<VmaAllocation>(SwapChain::MAX_FRAMES_IN_FLIGHT);
			std::vector<VmaAllocationInfo> allocationInfos = std::vector<VmaAllocationInfo>(SwapChain::MAX_FRAMES_IN_FLIGHT);
		};

		std::vector<VMABuffer> m_uniforms = std::vector<VMABuffer>(1);
		
		//std::vector<std::unique_ptr<Buffer>> m_uboBuffersTWO = std::vector<std::unique_ptr<Buffer>>(SwapChain::MAX_FRAMES_IN_FLIGHT);

		std::unique_ptr<DescriptorPool> m_globalDescriptorPool;
		std::unique_ptr<DescriptorSetLayout> m_globalSetLayout;
		std::vector<VkDescriptorSet> m_descriptorSets = std::vector<VkDescriptorSet>(SwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
	};
}