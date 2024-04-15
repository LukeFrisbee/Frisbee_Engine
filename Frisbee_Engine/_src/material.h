//#pragma once
// 
//#include "shader.h"
//#include <memory>
//
//namespace fengine {
//	struct PBR {
//		glm::vec3 albedo{ 1.0, 0.0, 0.0 };
//		float metallic = 0.0;
//		float roughness = 0.3;
//		float ao = 0.02;
//	};
//
//	class Material {
//		public:
//			Material(const Device& device, const VkRenderPass& renderPass, const VkDescriptorSetLayout& globalSetLayout, 
//				const char* vertexPath, const char* fragPath);
//			Material(std::shared_ptr<Shader> shader);
//
//			Material(const Material&) = delete;
//			Material& operator= (const Material&) = delete;
//
//			const std::shared_ptr<Shader> getShader() const { return m_shader; }
//			const PBR getPBR() const { return pbr; }
//
//		private:
//			PBR pbr{};
//			std::shared_ptr<Shader> m_shader;
//	};
//}