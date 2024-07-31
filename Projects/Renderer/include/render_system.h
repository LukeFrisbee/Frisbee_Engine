#pragma once

#include <memory>
#include <vector>

#include "device.h"
#include "pipeline.h"

#include "render_object.h"
#include "camera.h"
#include "frame_info.h"
#include "renderer_resources.h"

#include "shader.h"

namespace fengine {
	class RenderSystem {
		public:
			RenderSystem(Device& device, RendererResources& resources);
			
			RenderSystem(const RenderSystem&) = delete;
			RenderSystem& operator=(const RenderSystem&) = delete;

			void renderGameObjects(const FrameInfo& frameInfo) const;

		private:

			Device& m_device;
			RendererResources& m_resources;
			std::unique_ptr<FPipeline> m_wireframePipeline;
	};
}