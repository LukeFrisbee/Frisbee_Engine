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
//			Material(Shader& shader);
//
//
//			//Material(const Material&) = delete;
//			//Material& operator = (const Material&) = delete;
//
//			const std::shared_ptr<Shader> getShader() const { return m_shader; }
//
//		private:
//			std::vector<UBO> m_uniformBuffers;
//			std::shared_ptr<Shader> m_shader;
//	};
//}