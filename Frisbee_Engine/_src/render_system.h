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
			RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
			~RenderSystem();

			RenderSystem(const RenderSystem&) = delete;
			RenderSystem& operator=(const RenderSystem&) = delete;

			void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

		private:
			void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
			void createPipeline(VkRenderPass renderPass);

			Device& m_device;
			std::unique_ptr<FPipeline> m_fPipeline;
			std::unique_ptr<FPipeline> m_fPipelineWireFrame;
			VkPipelineLayout m_pipelineLayout;
	};
}