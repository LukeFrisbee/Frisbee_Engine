#include "renderer_resources.h"

#include <string.h>

namespace fengine {
    int RendererResources::createShader(const std::string& vertexFilePath,
        const std::string& fragmentFilePath,
        const Shader::Settings& settings,
        const std::vector<size_t>& uboSizes)
    {
        m_shaders.push_back(
            std::make_unique<Shader>(m_device,
                m_renderPass,
                settings,
                uboSizes,
                vertexFilePath,
                fragmentFilePath)
        );

        static unsigned int shaderId = 0;
        return shaderId++;
    }

    int RendererResources::createModel(const std::vector<Model::Vertex> vertices) {
        m_models.push_back(std::make_unique<Model>(m_device, vertices));
        return m_models.size() - 1;
    }

    int RendererResources::createModel(const std::vector<Model::Vertex> vertices, const std::vector<uint32_t> indicies) {
        m_models.push_back(std::make_unique<Model>(m_device, vertices, indicies));

        return m_models.size() - 1;
    }

    int RendererResources::createModel(const std::string& filePath) {
        m_models.push_back(std::make_unique<Model>(m_device, filePath));

        return m_models.size() - 1;
    }

    void RendererResources::updateModel(uint32_t modelId, const std::vector<Model::Vertex> vertices) {
        assert(modelId >= 0 && modelId < m_models.size() && "RendererResources: Cannot updateModel out of bounds!");

        m_models[modelId]->updateVertexBuffers(vertices);
    }

    uint32_t RendererResources::addRenderObject(RenderObject renderObject) {
        static uint32_t id = 0;
        auto newId = id++;
        ids.push_back(newId);
        m_renderObjects.push_back(std::move(renderObject));
        idToIndex[newId] = m_renderObjects.size() - 1;
        return newId;
    }

    RenderObject& RendererResources::getRenderObject(uint32_t id)
    {
        auto it = idToIndex.find(id);
        if (it != idToIndex.end()) {
            return m_renderObjects[it->second];
        }
    }

    void RendererResources::removeRenderObject(uint32_t id) {
        auto it = idToIndex.find(id);
        if (it != idToIndex.end()) {
            size_t indexToRemove = it->second;
            size_t lastIndex = m_renderObjects.size() - 1;

            // Swap with last element for efficient removal
            if (indexToRemove != lastIndex) {
                std::swap(m_renderObjects[indexToRemove], m_renderObjects[lastIndex]);
                std::swap(ids[indexToRemove], ids[lastIndex]);
                // Update map for the swapped element
                idToIndex[ids[indexToRemove]] = indexToRemove;
            }

            // Remove last element from vector
            m_renderObjects.pop_back();
            ids.pop_back();
            idToIndex.erase(id);
        }
    }
}