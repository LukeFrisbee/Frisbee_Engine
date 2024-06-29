#pragma once

#include "render_object.h"

#include "render_system.h"
#include "device.h"

namespace fengine {
    class RenderObjectHolder {
    public:
        RenderObjectHolder() = default;

        // Add a new RenderObject
        uint32_t addRenderObject(RenderObject renderObject) {
            static uint32_t id = 0;
            auto newId = id++;
            ids.push_back(newId);

            ids.push_back(newId);
            renderObjects.push_back(std::move(renderObject));
            idToIndex[newId] = renderObjects.size() - 1;
            return newId;
        }

        RenderObject& getRenderObject(uint32_t id)
        {
            auto it = idToIndex.find(id);
            if (it != idToIndex.end()) {
                return renderObjects[it->second];
            }
        }

        void removeRenderObject(uint32_t id) {
            auto it = idToIndex.find(id);
            if (it != idToIndex.end()) {
                size_t indexToRemove = it->second;
                size_t lastIndex = renderObjects.size() - 1;

                // Swap with last element for efficient removal
                if (indexToRemove != lastIndex) {
                    std::swap(renderObjects[indexToRemove], renderObjects[lastIndex]);
                    std::swap(ids[indexToRemove], ids[lastIndex]);
                    // Update map for the swapped element
                    idToIndex[ids[indexToRemove]] = indexToRemove;
                }

                // Remove last element from vector
                renderObjects.pop_back();
                ids.pop_back();
                idToIndex.erase(id);
            }
        }

        RenderObjectHolder(const RenderObjectHolder&) = delete;
        RenderObjectHolder& operator=(const RenderObjectHolder&) = delete;
        RenderObjectHolder(RenderObjectHolder&&) = delete;
        RenderObjectHolder& operator=(RenderObjectHolder&&) = delete;

    private:
        std::vector<RenderObject>& getRenderObjects() {
            return renderObjects;
        }

        std::vector<RenderObject> renderObjects;
        std::vector<uint32_t> ids;
        std::unordered_map<uint32_t, size_t> idToIndex; // Map ID to index in vector
        int nextId = 0; // For generating new IDs

        friend class App;
    };
}