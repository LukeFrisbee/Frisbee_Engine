#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "pipeline.h"
#include "buffer.h"
#include "swap_chain.h"
#include "descriptors.h"

namespace fengine {
	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
		//alignas(16) glm::vec3 Color{};
	};

	class Shader {
	public:
		// Alightment Requirements for UBO:
		// Multiples of 16
		// https://vulkan-tutorial.com/Uniform_buffers/Descriptor_pool_and_sets#page_Alignment-requirements
		struct GlobalUbo {
			glm::mat4 projectionViewMatrix{ 1.0f };
			glm::vec3 LightPos = glm::vec3(1.0f, -4.0f, -1.0f);
			alignas(16) glm::vec3 camPos = {};

			alignas(16) glm::vec3 albedo {1.0f, 0.0f, 0.0f };
			float metallic { 0.0f };
			float roughness { 0.3f };
			float ao { 0.02f };
		};

		Shader(Device& device, const VkRenderPass& renderPass, const std::string& vertexPath, const std::string& fragPath);
		~Shader();

		void bindPipeline(const VkCommandBuffer& commandBuffer);
		void bindDescriptorSets(const VkCommandBuffer& commandBuffer, int frameIndex);
		void updatePushConstants(const VkCommandBuffer& commandBuffer, const SimplePushConstantData& push);
		void updateDescriptorSets(GlobalUbo& ubo, int frameIndex);
		//void updateDescriptorSets(int frameIndex)

		//Shader(const Shader&) = delete;
		//Shader& operator= (const Shader&) = delete;

	private:
		void _buildDescriptorSet();
		void _createPipelineLayout();
		void _createPipeline(const VkRenderPass& renderPass, const std::string& vertexPath, const std::string& fragPath);

		Device& m_device;
		VkPipelineLayout m_pipelineLayout;
		std::unique_ptr<FPipeline> m_pipeline;
		std::vector<std::unique_ptr<Buffer>> m_uboBuffers = std::vector<std::unique_ptr<Buffer>>(SwapChain::MAX_FRAMES_IN_FLIGHT);

		std::unique_ptr<DescriptorPool> m_globalDescriptorPool;
		std::unique_ptr<DescriptorSetLayout> m_globalSetLayout;
		std::vector<VkDescriptorSet> m_descriptorSets = std::vector<VkDescriptorSet>(SwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
	};
}