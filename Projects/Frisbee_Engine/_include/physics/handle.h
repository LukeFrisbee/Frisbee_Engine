#pragma once

namespace physics {
    class Handle {
    public:
        Handle() = delete;
        Handle(uint32_t idx, uint32_t gen) : index(idx), generation(gen) {}

        uint32_t getIndex() const { return index; }
        uint32_t getGeneration() const { return generation; }

        bool operator==(const Handle& other) const {
            return index == other.index && generation == other.generation;
        }

        bool operator!=(const Handle& other) const {
            return !(*this == other);
        }

        bool operator<(const Handle& other) const {
            return index < other.index || (index == other.index && generation < other.generation);
        }

    private:
        uint32_t index;
        uint32_t generation;
    };
}