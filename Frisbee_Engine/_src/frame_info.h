#pragma once

#include <vulkan/vulkan.h>

#include "camera.h"

namespace fengine {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
	};
}  