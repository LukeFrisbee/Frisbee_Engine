#pragma once

#include <memory>
#include <vector>
#include <cassert>

#include "window.h"
#include "swap_chain.h"
#include "f_imgui_draw.h"

namespace fengine { 
	class Renderer {

		public:
			
			Renderer(FWindow& window, Device& device);
			~Renderer();

			Renderer(const Renderer &) = delete;
			Renderer& operator=(const Renderer&) = delete;

			// getters
			bool isFrameInProgress() const { return m_isFrameStarted; }
			VkCommandBuffer getCurrentCommandBuffer() const { 
				assert(m_isFrameStarted && "Cannot getCommandBuffer while a frame is not in progress");
				return m_commandBuffers[m_currentFrameIndex]; 
			}
			int getFrameIndex() const 
			{
				assert(m_isFrameStarted && "Cannot getFrameIndex while a frame is not in progress");
				return m_currentFrameIndex; 
			}
			VkRenderPass getRenderPass() const { return m_swapChain->getRenderPass(); }
			float getAspectRatio() const { return m_swapChain->extentAspectRatio(); }

			VkCommandBuffer beginFrame();
			void endFrame();
			void beginRenderPass(VkCommandBuffer commandBuffer);
			void endRenderPass(VkCommandBuffer commandBuffer);

		private:
			uint32_t m_currentImageIndex;
			int m_currentFrameIndex{};
			bool m_isFrameStarted = false;

			FWindow& m_window;
			Device& m_device;
			std::unique_ptr<SwapChain> m_swapChain;
			std::vector<VkCommandBuffer> m_commandBuffers;
			std::unique_ptr<FImguiDraw> m_draw;

			void createCommandBuffers();
			void freeCommandBuffers();
			void recreateSwapChain();
	};
}