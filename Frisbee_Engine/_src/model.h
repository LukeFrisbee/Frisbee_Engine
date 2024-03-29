#pragma once

#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include "device.h"
#include "buffer.h"

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

			struct Builder {
				std::vector<Vertex> vertices{};
				std::vector<uint32_t> indices{};

				void loadModel(const std::string& fileName);
			};

			Model(Device& fDevice, const Model::Builder& builder);
			~Model();

			Model(const Model&) = delete;
			Model& operator= (const Model&) = delete;

			static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filePath);

			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);

		private:
			Device& m_device;

			std::unique_ptr<Buffer> m_vertexBuffer;
			uint32_t m_vertexCount;

			bool hasIndexBuffer = false;
			std::unique_ptr<Buffer> m_indexBuffer;
			uint32_t m_indexCount;

			void createVertexBuffers(const std::vector<Vertex>& vertices);
			void createIndexBuffers(const std::vector<uint32_t>& indices);
	};
}