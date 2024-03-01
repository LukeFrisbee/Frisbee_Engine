#pragma once

#include <memory>
#include <vector>

#include "device.h"
#include "pipeline.h"

#include "game_object.h"
#include "camera.h"
#include "frame_info.h"

namespace fengine {
	class RenderSystem {
		public:
			// Alightment Requirements for UBO:
			// Multiples of 16
			// https://vulkan-tutorial.com/Uniform_buffers/Descriptor_pool_and_sets#page_Alignment-requirements
			struct GlobalUbo {
				glm::mat4 projectionViewMatrix{ 1.0f };
				glm::vec3 LightPos = glm::vec3(1.0f, -4.0f, -1.0f);
				alignas(16) glm::vec3 camPos = {};

				alignas(16) glm::vec3 albedo { 1.0, 0.0, 0.0 };
				float metallic = 0.0;
				float roughness = 0.3;
				float ao = 0.02;
			};

			struct SimplePushConstantData {
				glm::mat4 modelMatrix{ 1.0f };
				glm::mat4 normalMatrix{ 1.0f };
				//alignas(16) glm::vec3 Color{};
			};

			RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
			~RenderSystem();

			RenderSystem(const RenderSystem&) = delete;
			RenderSystem& operator=(const RenderSystem&) = delete;

			void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

		private:
			void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
			void createPipeline(VkRenderPass renderPass);

			Device& m_device;
			// TODO: Pipeline derivative or Cache (Instead of rebuilding a pipeline from scratch)
			std::unique_ptr<FPipeline> m_fPipelinePBR;
			std::unique_ptr<FPipeline> m_fPipelineWhite;
			std::unique_ptr<FPipeline> m_fPipelineWireFrame;
			VkPipelineLayout m_pipelineLayout;
	};
}