#pragma once

#include "imgui_impl_vulkan.h"

#include "glm/glm.hpp"

#include "global_data.h"

#include "device.h"
#include "window.h"
#include "swap_chain.h"

namespace fengine {
	class FImguiDraw {
		struct UIInputValues {
			glm::vec2 position{};
			glm::vec3 Color{1.0f, 1.0f, 1.0f};
		};

		public:
			//FImguiDraw() = delete;
			FImguiDraw(FWindow& window, Device& device, uint32_t imageCount, VkRenderPass renderPass);
			~FImguiDraw();

			void DrawFrame();
			void RenderDrawData(VkCommandBuffer commandBuffer);

		private:
			void SettingsWindow(ImGuiIO& io, GlobalData& globalData);
			void initImguiStyle();
	};
}