#pragma once

#include <memory>
#include <vector>

#include "device.h"
#include "pipeline.h"

#include "render_object.h"
#include "camera.h"
#include "frame_info.h"

#include "shader.h"

namespace fengine {
	class RenderSystem {
		public:
			RenderSystem(Device& device, const VkRenderPass& renderPass);
			
			RenderSystem(const RenderSystem&) = delete;
			RenderSystem& operator=(const RenderSystem&) = delete;

			int createShader(const std::string& vertexFilePath, 
				const std::string& fragmentFilePath, 
				const Shader::Settings& settings,
				const std::vector<size_t>& uboSizes);
			void renderGameObjects(FrameInfo& frameInfo, std::vector<RenderObject>& gameObjects);

		private:

			Device& m_device;
			const VkRenderPass& m_renderPass;

			std::unique_ptr<FPipeline> m_wireframePipeline;
			std::vector<std::unique_ptr<Shader>> m_shaders;
	};
}