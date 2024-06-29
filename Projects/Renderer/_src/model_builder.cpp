#include "model_builder.h"

#include <vector>
#include <unordered_map>

//#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <stdexcept>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "frisbee_utils.h"

namespace std {
	template<>
	struct hash<fengine::Model::Vertex> {
		size_t operator()(fengine::Model::Vertex const& vertex) const {
			size_t seed = 0;
			utils::hashCombine(seed, vertex.position, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace fengine {
	std::unique_ptr<Model> ModelBuilder::BuildModelFromFile(const std::string& filePath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string error;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filePath.c_str())) {
			throw std::runtime_error(warn + error);
		}

		Model::Data data{};
		data.vertices.clear();
		data.indices.clear();

		std::unordered_map<Model::Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Model::Vertex vertex{};

				// Position
				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};
				}

				// Normals
				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				// UV
				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(data.vertices.size());
					data.vertices.push_back(vertex);
				}
				data.indices.push_back(uniqueVertices[vertex]);
			}
		}
		return std::make_unique<Model>(m_device, data);
	}
}
