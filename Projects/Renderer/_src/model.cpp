#include "model.h"

#include <cassert>
#include <cstring>
#include <unordered_map>

#include "frisbee_utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

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

	Model::Model(Device& fDevice, const Model::Data& data) : m_device{ fDevice }
	{
		createVertexBuffers(data.vertices);
		createIndexBuffers(data.indices);
	}

	Model::Model(Device& fDevice, const std::string& filePath) : m_device{ fDevice }
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
			

		createVertexBuffers(data.vertices);
		createIndexBuffers(data.indices);
	}

	Model::~Model()
	{
		if (m_vertexBuffer != VK_NULL_HANDLE && m_vertexBufferAllocation != VK_NULL_HANDLE) {
			vmaDestroyBuffer(m_device.vmaAllocator(), m_vertexBuffer, m_vertexBufferAllocation);
			m_vertexBuffer = VK_NULL_HANDLE;
			m_vertexBufferAllocation = VK_NULL_HANDLE;
		}
		if (hasIndexBuffer && m_indexBuffer != VK_NULL_HANDLE && m_indexBufferAllocation != VK_NULL_HANDLE) {
			vmaDestroyBuffer(m_device.vmaAllocator(), m_indexBuffer, m_indexBufferAllocation);
			m_indexBuffer = VK_NULL_HANDLE;
			m_indexBufferAllocation = VK_NULL_HANDLE;
		}
	}

	std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attributeDescriptions;
	}

	void Model::bind(VkCommandBuffer commandBuffer)
	{
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_vertexBuffer, offsets);
		
		if (hasIndexBuffer)
			vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void Model::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer)
			vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
		else
			vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
	}

	void Model::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_vertexCount >= 3 && "Vertex count must be greater than 3");
		size_t bufferSize = sizeof(Vertex) * m_vertexCount;
		
		// Staging Buffer
		VkBufferCreateInfo stagingBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		stagingBufferInfo.size = bufferSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VmaAllocationCreateInfo stagingAllocCreateInfo = {};
		stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VkBuffer stagingBuffer;
		VmaAllocation stagingAlloc;
		vmaCreateBuffer(m_device.vmaAllocator(), &stagingBufferInfo, &stagingAllocCreateInfo, &stagingBuffer, &stagingAlloc, nullptr);
		
		void* mappedData;
		vmaMapMemory(m_device.vmaAllocator(), stagingAlloc, &mappedData);
		memcpy(mappedData, vertices.data(), bufferSize);
		vmaUnmapMemory(m_device.vmaAllocator(), stagingAlloc);

		// Vertex Buffer
		VkBufferCreateInfo vertexBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		vertexBufferInfo.size = bufferSize;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VmaAllocationCreateInfo vertexAllocCreateInfo = {};
		vertexAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		vertexAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		vmaCreateBuffer(m_device.vmaAllocator(), &vertexBufferInfo, &vertexAllocCreateInfo, &m_vertexBuffer, &m_vertexBufferAllocation, nullptr);

		m_device.copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

		vmaDestroyBuffer(m_device.vmaAllocator(), stagingBuffer, stagingAlloc);
	}

	void Model::updateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		auto vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount == m_vertexCount && "Cannot updates VertexBuffers if vertex count is not the same");
		size_t bufferSize = sizeof(Vertex) * m_vertexCount;

		// Staging Buffer
		VkBufferCreateInfo stagingBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		stagingBufferInfo.size = bufferSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VmaAllocationCreateInfo stagingAllocCreateInfo = {};
		stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VkBuffer stagingBuffer;
		VmaAllocation stagingAlloc;
		vmaCreateBuffer(m_device.vmaAllocator(), &stagingBufferInfo, &stagingAllocCreateInfo, &stagingBuffer, &stagingAlloc, nullptr);

		void* mappedData;
		vmaMapMemory(m_device.vmaAllocator(), stagingAlloc, &mappedData);
		memcpy(mappedData, vertices.data(), bufferSize);
		vmaUnmapMemory(m_device.vmaAllocator(), stagingAlloc);

		// Update existing Vertex Buffer
		void* vertexBufferMappedData;
		vmaMapMemory(m_device.vmaAllocator(), m_vertexBufferAllocation, &vertexBufferMappedData);
		memcpy(vertexBufferMappedData, vertices.data(), bufferSize);
		vmaUnmapMemory(m_device.vmaAllocator(), m_vertexBufferAllocation);

		// Or alternatively, using a staging buffer to update the vertex buffer
		m_device.copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

		vmaDestroyBuffer(m_device.vmaAllocator(), stagingBuffer, stagingAlloc);
	}

	void Model::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = m_indexCount > 0;

		if (!hasIndexBuffer)
			return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		// Staging Buffer
		VkBufferCreateInfo stagingBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		stagingBufferInfo.size = bufferSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VmaAllocationCreateInfo stagingAllocCreateInfo = {};
		stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VkBuffer stagingBuffer;
		VmaAllocation stagingAlloc;
		VmaAllocationInfo stagingAllocInfo;
		vmaCreateBuffer(m_device.vmaAllocator(), &stagingBufferInfo, &stagingAllocCreateInfo, &stagingBuffer, &stagingAlloc, &stagingAllocInfo);

		memcpy(stagingAllocInfo.pMappedData, indices.data(), bufferSize);
		//vmaCreateBuffer(m_device.vmaAllocator(), &stagingBufferInfo, &stagingAllocCreateInfo, &stagingBuffer, &stagingAlloc, nullptr);

		//void* mappedData;
		//vmaMapMemory(m_device.vmaAllocator(), stagingAlloc, &mappedData);
		//memcpy(mappedData, indices.data(), bufferSize);
		//vmaUnmapMemory(m_device.vmaAllocator(), stagingAlloc);

		// Vertex Buffer
		VkBufferCreateInfo indexBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		indexBufferInfo.size = bufferSize;
		indexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VmaAllocationCreateInfo indexAllocCreateInfo = {};
		indexAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		indexAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		vmaCreateBuffer(m_device.vmaAllocator(), &indexBufferInfo, &indexAllocCreateInfo, &m_indexBuffer, &m_indexBufferAllocation, nullptr);

		m_device.copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);
		vmaDestroyBuffer(m_device.vmaAllocator(), stagingBuffer, stagingAlloc);
	}
}