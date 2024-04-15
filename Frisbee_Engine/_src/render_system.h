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
			RenderSystem(Device& device, const VkRenderPass& renderPass);
			
			RenderSystem(const RenderSystem&) = delete;
			RenderSystem& operator=(const RenderSystem&) = delete;

			void createShader(const std::string& vertexFilePath, const std::string& fragmentFilePath);
			void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

		private:

			Device& m_device;
			const VkRenderPass& m_renderPass;

			std::unique_ptr<FPipeline> m_wireframePipeline;
			std::vector<std::unique_ptr<Shader>> m_shaders;
	};
}