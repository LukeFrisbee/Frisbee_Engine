#pragma once

#include <memory>
#include <vector>

#include "device.h"
#include "pipeline.h"

#include "game_object.h"
#include "camera.h"
#include "frame_info.h"

#include "shader.h"

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
			};

			RenderSystem(Device& device, VkRenderPass renderPass);
			
			RenderSystem(const RenderSystem&) = delete;
			RenderSystem& operator=(const RenderSystem&) = delete;

			void createShader();
			void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

		private:
			//void _makeWireframe(const VkRenderPass& renderPass, const VkDescriptorSetLayout& descriptorSetLayout);

			Device& m_device;
			VkRenderPass& m_renderPass;

			std::unique_ptr<FPipeline> m_wireframePipeline;
			std::vector<std::unique_ptr<Shader>> m_shaders;

	};
}