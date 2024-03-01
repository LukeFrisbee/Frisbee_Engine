#include "f_imgui_draw.h"

#include <stdexcept>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "glm/gtx/string_cast.hpp"

namespace fengine {
	FImguiDraw::FImguiDraw(FWindow& window, Device& device, uint32_t imageCount, VkRenderPass renderPass)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Config
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = device.instance();
		init_info.PhysicalDevice = device.physicalDevice();
		init_info.Device = device.device();
		init_info.QueueFamily = device.findPhysicalQueueFamilies().graphicsFamily;
		init_info.Queue = device.graphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = device.descriptorPool();
		init_info.Subpass = 0;
		init_info.MinImageCount = imageCount;
		init_info.ImageCount = imageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		init_info.CheckVkResultFn = nullptr;
		ImGui_ImplVulkan_Init(&init_info, renderPass);
		ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);

		initImguiStyle();
	}

	FImguiDraw::~FImguiDraw()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void FImguiDraw::DrawFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGuiIO& io = ImGui::GetIO();
		auto& globalData = GlobalData::getInstance();

		ImGui::NewFrame();

		SettingsWindow(io, globalData);
		//ImGui::ShowDemoWindow();

		ImGui::Render();
	}

	void FImguiDraw::RenderDrawData(VkCommandBuffer commandBuffer)
	{
		auto drawData = ImGui::GetDrawData();

		if (drawData == nullptr)
		{
			throw std::runtime_error("Trying to Render Imgui draw data when no draw data exists!");
			return;
		}

		ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
	}

	void FImguiDraw::SettingsWindow(ImGuiIO& io, GlobalData& globalData)
	{
		ImGui::Begin("Settings");
		ImGui::SetWindowPos({ 10, 10 }, ImGuiCond_Once);
		auto width = io.DisplaySize.x / 3.8f;
		auto height = io.DisplaySize.y / 1.5f;
		ImGui::SetWindowSize({ width, height }, ImGuiCond_Once);

		// Info
		if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto fpsText = "FPS: " + std::to_string(globalData.fps);
			ImGui::Text(fpsText.c_str());

			ImGui::Text("Camera Position:");
			ImGui::Text(glm::to_string(globalData.camPos).c_str());

			ImGui::Text("Camera Rotation:");
			ImGui::Text(glm::to_string(globalData.camRot).c_str());
		}

		// Pipeline
		if (ImGui::CollapsingHeader("Pipeline", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::ColorEdit4("Color", (float*)&globalData.albedo);

			ImGui::DragFloat("Roughness", &globalData.roughness, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &globalData.metallic, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Ao", &globalData.ao, 0.01f, 0.0f, 1.0f);

			float metallic = 0.0;
			float roughness = 0.3;
			float ao = 0.02;
			ImGui::Checkbox("Wireframe", &globalData.isWireFrame);
		}

		// Swap Chain
		//ImGui::Spacing();
		if (ImGui::CollapsingHeader("Swap Chain", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto presentModeText = "Present Mode: " + globalData.swapPresetMode;
			ImGui::Text(presentModeText.c_str());
		}

		// IMGUI Inputs
		if (ImGui::CollapsingHeader("Imgui Inputs", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::IsMousePosValid())
				ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
			else
				ImGui::Text("Mouse pos: <INVALID>");
			struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
			ImGuiKey start_key = (ImGuiKey)0;
			ImGui::Text("Keys down:");         for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue; ImGui::SameLine(); ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key); }
			ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
		}

		ImGui::End();
	}

	void FImguiDraw::initImguiStyle()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Font
		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Users\\lukeb\\_Projects\\C++\\Frisbee_Engine\\Frisbee_Engine\\fonts\\Roboto\\Roboto-Regular.ttf", 30.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		io.FontDefault = font;
		//io.FontGlobalScale = 1.5f;

		// Colors
		{
			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
			colors[ImGuiCol_Border] = ImVec4(0.02f, 0.02f, 0.02f, 0.3f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
			colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
			colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
			colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
			colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
			colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
			colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
			colors[ImGuiCol_NavHighlight] = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.00f, 0.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
		}

		// Style
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowPadding = ImVec2(8.00f, 8.00f);
			style.FramePadding = ImVec2(5.00f, 2.00f);
			style.CellPadding = ImVec2(6.00f, 6.00f);
			style.ItemSpacing = ImVec2(6.00f, 6.00f);
			style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
			style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
			style.IndentSpacing = 25;
			style.ScrollbarSize = 15;
			style.GrabMinSize = 10;
			style.WindowBorderSize = 1;
			style.ChildBorderSize = 1;
			style.PopupBorderSize = 1;
			style.FrameBorderSize = 1;
			style.TabBorderSize = 1;
			style.WindowRounding = 7;
			style.ChildRounding = 4;
			style.FrameRounding = 3;
			style.PopupRounding = 4;
			style.ScrollbarRounding = 9;
			style.GrabRounding = 3;
			style.LogSliderDeadzone = 4;
			style.TabRounding = 4;
		}
	}
}