#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "render_object.h"

namespace fengine {
	class RendererResources {
	public:
        RendererResources(Device& device, const VkRenderPass& renderPass)
            : m_device{ device }, m_renderPass(renderPass) {};

        int createShader(const std::string& vertexFilePath,
            const std::string& fragmentFilePath,
            const Shader::Settings& settings,
            const std::vector<size_t>& uboSizes);

        int createModel(const Model::Data& data);
        int createModel(const std::string& filePath);

        // TODO: allow for index buffers to be updated aswell (only uses data.vertices)
        void updateModel(uint32_t modelId, const Model::Data& data);

        uint32_t addRenderObject(RenderObject renderObject);
        RenderObject& getRenderObject(uint32_t id);
        std::vector<uint32_t> getIdsCopy() { return ids; }
        void removeRenderObject(uint32_t id);

        RendererResources(const RendererResources&) = delete;
        RendererResources& operator=(const RendererResources&) = delete;
        RendererResources(RendererResources&&) = delete;
        RendererResources& operator=(RendererResources&&) = delete;

	private:
        Device& m_device;
        const VkRenderPass& m_renderPass;

        std::vector<RenderObject> m_renderObjects;
        std::vector<uint32_t> ids;
        std::unordered_map<uint32_t, size_t> idToIndex; // Map ID to index in vector
        int nextId = 0; // For generating new IDs

		std::vector<std::unique_ptr<Shader>> m_shaders;
		std::vector<std::unique_ptr<Model>> m_models;

		friend class RenderSystem;
	};
}