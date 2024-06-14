#pragma once

#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include "device.h"

namespace fengine {
	class Model {
		public:
		struct Vertex {
				glm::vec3 position{};
				glm::vec3 normal{};
				glm::vec2 uv{};

				static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
				static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

				bool operator==(const Vertex& other) const {
					return position == other.position
						&& normal == other.normal
						&& uv == other.uv;
				}
			};

			struct Data {
				std::vector<Vertex> vertices{};
				std::vector<uint32_t> indices{};
			};

			Model(Device& fDevice, const Model::Data& data);
			~Model();

			Model(const Model&) = delete;
			Model& operator= (const Model&) = delete;

			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);


			void updateVertexBuffers(const std::vector<Vertex>& vertices);

		private:
			void createVertexBuffers(const std::vector<Vertex>& vertices);
			void createIndexBuffers(const std::vector<uint32_t>& indices);

			Device& m_device;

			VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
			VmaAllocation m_vertexBufferAllocation = VK_NULL_HANDLE;
			uint32_t m_vertexCount = 0;

			VkBuffer m_indexBuffer = VK_NULL_HANDLE;
			VmaAllocation m_indexBufferAllocation = VK_NULL_HANDLE;
			uint32_t m_indexCount = 0;
			bool hasIndexBuffer = false;

	};
}