//#include "material.h"
//
//namespace fengine 
//{
//	Material::Material(const Device& device, const VkRenderPass& renderPass, const VkDescriptorSetLayout& globalSetLayout, 
//		const char* vertexPath, const char* fragPath) {
//
//		m_shader = std::make_shared<Shader>(
//			device,
//			renderPass,
//			globalSetLayout,
//			vertexPath,
//			fragPath
//		);
//	}
//
//	Material::Material(std::shared_ptr<Shader> shader) : m_shader{ shader }  
//	{
//	}
//}