#include "f_imgui_draw.h"

#include <string>
#include <filesystem>
#include <cassert>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "ImGuizmo.h"

#include "components/transform.h"

namespace fengine {
	EditorUI::EditorUI(FWindow& window, Device& device, size_t imageCount, VkRenderPass renderPass, Camera& camera, RendererResources& rendererResources, entt::registry& ecs)
		: m_camera{ camera }, m_rendererResources { rendererResources }, m_ecs { ecs }
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Config
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
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
		init_info.MinImageCount = static_cast<uint32_t>(imageCount);
		init_info.ImageCount = static_cast<uint32_t>(imageCount);
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		init_info.CheckVkResultFn = nullptr;
		ImGui_ImplVulkan_Init(&init_info, renderPass);
		ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);

		initImguiStyle();
	}

	EditorUI::~EditorUI()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorUI::LogWindow() {
		ImGui::Begin("Log");

		ImGui::TextWrapped(logString.c_str());
		logString = "";

		ImGui::End();
	}

	void EditorUI::Gizmos() 
	{
		static ImGuizmo::OPERATION s_gizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE s_gizmoMode(ImGuizmo::WORLD);

		if (ImGui::IsKeyPressed(ImGuiKey_1))
			s_gizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_2))
			s_gizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_3)) 
			s_gizmoOperation = ImGuizmo::SCALE;

		if (ImGui::IsKeyPressed(ImGuiKey_4) ) {
			if (s_gizmoMode == ImGuizmo::WORLD)
				s_gizmoMode = ImGuizmo::LOCAL;
			else
				s_gizmoMode = ImGuizmo::WORLD;
		}

		ImGuizmo::BeginFrame();

		auto projection = m_camera.getProjection();
		projection[1][1] *= -1.0f;

		auto* transform = m_ecs.try_get<Transform>(m_selectedEntity);
		if (transform != nullptr) {
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform->translation);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform->scale);
			glm::mat4 rotationMatrix = glm::toMat4(glm::quat(transform->rotation));
			glm::mat4 matrix = translationMatrix * rotationMatrix * scaleMatrix;

			//glm::mat4 matrix{ 1.0f };

			if (ImGuizmo::Manipulate(glm::value_ptr(m_camera.getView()), glm::value_ptr(projection),
				s_gizmoOperation, s_gizmoMode, glm::value_ptr(matrix)))
			{
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::quat orientation;
				glm::decompose(matrix, transform->scale, orientation, transform->translation, skew, perspective);
				transform->rotation = glm::degrees(glm::eulerAngles(orientation));

				auto* mesh = m_ecs.try_get<RenderId>(m_selectedEntity);
				if (mesh != nullptr) {
					auto& render = m_rendererResources.getRenderObject(mesh->id);
					render.transform.translation = transform->translation;
					render.transform.rotation = transform->rotation;
					render.transform.scale = transform->scale;
				}
			}
		}

		ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

		// Invert projection Y
		//auto projection = m_camera.getProjection();
		//projection[1][1] *= -1.0f;
		//RenderObject& object = m_rendererResources.getRenderObject(selectedRenderId);
		//glm::mat4 modelMatrix = object.transform.modelMatrix();
		//
		//if (ImGuizmo::Manipulate(glm::value_ptr(m_camera.getView()), glm::value_ptr(projection),
		//	s_gizmoOperation, s_gizmoMode, glm::value_ptr(modelMatrix)))
		//{
		//	glm::vec3 scale;
		//	glm::quat rotation;
		//	glm::vec3 translation;
		//	glm::vec3 skew;
		//	glm::vec4 perspective;
		//	glm::decompose(modelMatrix, scale, rotation, translation, skew, perspective);
		//	glm::vec3 eulerAngles = glm::eulerAngles(rotation);
		//
		//	switch (s_gizmoOperation) {
		//	case ImGuizmo::TRANSLATE:
		//		object.transform.translation = translation;
		//		break;
		//	case ImGuizmo::ROTATE:
		//		object.transform.rotation = eulerAngles;
		//		break;
		//	case ImGuizmo::SCALE:
		//		object.transform.scale = scale;
		//		break;
		//	}
		//}
		//// Display the gizmo
	}
	
	void EditorUI::SettingsWindow()
	{
		ImGui::Begin("Settings");
		auto& globalData = GlobalData::getInstance();
		auto io = ImGui::GetIO();

		// Debug
		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text(globalData.debugString.c_str());
			globalData.debugString = "";

			ImGui::DragFloat3("LineStart", (float*)&globalData.lineStart, 0.1f);
			ImGui::DragFloat3("LineEnd", (float*)&globalData.lineEnd, 0.1f);
		}

		// Info
		if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto fpsText = "FPS: " + std::to_string(globalData.fps);
			ImGui::Text(fpsText.c_str());

			ImGui::Text("Camera Position:");
			ImGui::Text(glm::to_string(globalData.camPos).c_str());

			ImGui::Text("Camera Rotation:");
			ImGui::Text(glm::to_string(globalData.camRot).c_str());

			ImGui::Text("3D Mouse Position:");
			ImGui::Text(glm::to_string(globalData.mousePos).c_str());
		}

		// Pipeline
		if (ImGui::CollapsingHeader("Pipeline", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::ColorEdit3("Color", (float*)&globalData.albedo);

			ImGui::DragFloat("Metallic", &globalData.metallic, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Roughness", &globalData.roughness, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Ao", &globalData.ao, 0.01f, 0.0f, 1.0f);

			float metallic = 0.0;
			float roughness = 0.3;
			float ao = 0.02;
			ImGui::Checkbox("Wireframe", &globalData.isWireFrame);
		}

		// Swap Chain
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

	void EditorUI::EntitiesWindow() 
	{
		ImGui::Begin("Entities");

		auto view = m_ecs.view<Name>();

		for (auto entity : view) {
			auto& name = view.get<Name>(entity);
			auto nameText = name.name.c_str();

			// Calculate the size of the text
			ImVec2 textSize = ImGui::CalcTextSize(nameText);
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		
			// Define the clickable rectangle dimensions
			ImVec2 min = cursorPos;
			ImVec2 max = ImVec2(min.x + ImGui::GetWindowContentRegionMax().x, min.y + textSize.y);
			ImVec2 size = ImVec2(max.x - min.x, max.y - min.y);
		
			// Add an invisible button to handle clicks
			ImGui::SetCursorScreenPos(min);
			if (ImGui::InvisibleButton(nameText, size)) {
				m_selectedEntity = entity;
			}
		
			// Highlight the rectangle if it is the selected item
			if (m_selectedEntity == entity) {
				ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(10, 50, 10, 255));
			}
		
			// Render the text
			ImGui::SetCursorScreenPos(cursorPos); // Reset the cursor position to render text
			ImGui::Text(nameText);
		}

		//for (auto id : m_rendererResources.getIdsCopy()) {
		//	// Calculate the size of the text
		//	ImVec2 textSize = ImGui::CalcTextSize(renderIDToName[id].c_str());
		//	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		//
		//	// Define the clickable rectangle dimensions
		//	ImVec2 min = cursorPos;
		//	ImVec2 max = ImVec2(min.x + ImGui::GetWindowContentRegionMax().x, min.y + textSize.y);
		//	ImVec2 size = ImVec2(max.x - min.x, max.y - min.y);
		//
		//	// Add an invisible button to handle clicks
		//	ImGui::SetCursorScreenPos(min);
		//	if (ImGui::InvisibleButton(std::to_string(id).c_str(), size)) {
		//		selectedRenderId = id;
		//	}
		//
		//	// Highlight the rectangle if it is the selected item
		//	if (id == selectedRenderId) {
		//		ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(10, 50, 10, 255));
		//	}
		//
		//	// Render the text
		//	ImGui::SetCursorScreenPos(cursorPos); // Reset the cursor position to render text
		//	ImGui::Text(renderIDToName[id].c_str());
		//}

		ImGui::End();
	}

	void EditorUI::InspectorWindow() 
	{
		ImGui::Begin("Inspector");

		if (m_selectedEntity != entt::null) {
			auto* name = m_ecs.try_get<Name>(m_selectedEntity);
			if (name != nullptr) {
				ImGui::Text(name->name.c_str());
			}

			auto* transform = m_ecs.try_get<Transform>(m_selectedEntity);
			if (transform != nullptr) {
				ImGui::PushItemWidth(-1);
				ImGui::InputFloat3("T", glm::value_ptr(transform->translation));
				ImGui::PopItemWidth();

				ImGui::PushItemWidth(-1);
				ImGui::InputFloat3("R", glm::value_ptr(transform->rotation));
				ImGui::PopItemWidth();

				ImGui::PushItemWidth(-1);
				ImGui::InputFloat3("S", glm::value_ptr(transform->scale));
				ImGui::PopItemWidth();

				auto* mesh = m_ecs.try_get<RenderId>(m_selectedEntity);
				if (mesh != nullptr) {
					auto& render = m_rendererResources.getRenderObject(mesh->id);
					render.transform.translation = transform->translation;
					render.transform.rotation = transform->rotation;
					render.transform.scale = transform->scale;
				}
			}
		}

		//auto& object = m_rendererResources.getRenderObject(selectedRenderId);
		//
		//ImGui::Text(renderIDToName[selectedRenderId].c_str());
		//
		//ImGui::PushItemWidth(-1);
		//ImGui::InputFloat3("T", glm::value_ptr(object.transform.translation));
		//ImGui::PopItemWidth();
		//
		//ImGui::PushItemWidth(-1);
		//ImGui::InputFloat3("R", glm::value_ptr(object.transform.rotation));
		//ImGui::PopItemWidth();
		//
		//ImGui::PushItemWidth(-1);
		//ImGui::InputFloat3("S", glm::value_ptr(object.transform.scale));
		//ImGui::PopItemWidth();

		ImGui::End();
	}

	void EditorUI::BarWindow() {
		ImGui::Begin("Bar", NULL, ImGuiWindowFlags_NoTitleBar);

		auto& global = GlobalData::getInstance();

		std::vector<float> vec;
		std::queue<int> temp = global.fpsQueue;
		while (!temp.empty()) {
			vec.push_back(temp.front());
			temp.pop();
		}
		ImGui::SetNextItemWidth(100);
		ImGui::PlotLines("##Frame Times", vec.data(), static_cast<int>(vec.size()));

		ImGui::SameLine();
		auto fpsText = "fps: " + std::to_string(global.fps);
		ImGui::Text(fpsText.c_str());

		ImGui::End();
	}

	void EditorUI::DrawFrame(VkCommandBuffer commandBuffer)
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		LogWindow();
		SettingsWindow();
		EntitiesWindow();
		InspectorWindow();
		BarWindow();
		Gizmos();

		//ImGui::ShowDemoWindow();

		ImGui::Render();

		auto drawData = ImGui::GetDrawData();
		assert(drawData != nullptr && "Trying to Render Imgui draw data when no draw data exists!");
		ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
	}

	void EditorUI::initImguiStyle()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Font
		std::string fontFile = CMAKE_SOURCE_DIR + std::string("\\Projects\\Renderer\\fonts\\Roboto\\Roboto-Regular.ttf");
		ImFont* font = io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 30.0f, NULL, io.Fonts->GetGlyphRangesDefault());
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
			colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 0.40f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.40f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.40f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.40f, 0.00f, 1.00f);
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