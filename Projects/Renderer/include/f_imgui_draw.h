#pragma once

#include <string>
#include <unordered_map>

#include "imgui_impl_vulkan.h"
#include "glm/glm.hpp"

#include "global_data.h"
#include "device.h"
#include "window.h"
#include "swap_chain.h"
#include "renderer_resources.h"
#include "camera.h"

namespace fengine {
	class EditorUI {
		public:
			EditorUI(FWindow& window, Device& device, size_t imageCount, VkRenderPass renderPass, Camera& camera, RendererResources& rendererResources);
			~EditorUI();

			void DrawFrame(VkCommandBuffer commandBuffer);
			void LinkRenderObject(uint32_t renderId, std::string name)
			{ 
				renderIDToName[renderId] = name;
			}

			void Log(const std::string& msg) {
				logString += msg;
			}
			void Flush() {

			}

		private:
			uint32_t selectedRenderId{};

			std::string logString{};

			std::unordered_map<uint32_t, std::string> renderIDToName;
			RendererResources& m_rendererResources;
			Camera& m_camera;

			void LogWindow();
			void SettingsWindow();
			void ObjectsWindow();
			void InspectorWindow();
			void BarWindow();
			void Gizmos();

			void initImguiStyle();
	};
}