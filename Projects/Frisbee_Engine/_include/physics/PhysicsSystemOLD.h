#pragma once

#include "logger.h"
#include "handle.h"

namespace physics {
	class PhysicsSystem {

	public:
		PhysicsSystem() = default;

		Handle addSphere(const glm::vec3 position, const float radius) {
			int32_t index;
			if (!freeList.empty()) {
				index = freeList.back();
				freeList.pop_back();
				generations[index]++;
				spheres[index] = { position, radius };
			}
			else {
				index = spheres.size();
				spheres.push_back({position, radius});
				generations.push_back(0);
			}
			return Handle(index, generations[index]);
		}

		void removeSphere(const Handle& handle) {
			if (_isValid(handle)) {
				int32_t index = handle.getIndex();
				generations[index]++;
				freeList.push_back(index);
			}
		}

		std::vector<glm::vec3> lineCast(const glm::vec3& start, const glm::vec3& end) {
			std::vector<glm::vec3> positions;

			for (auto& sphere : spheres) {
				auto linePoint = _closestPointOnLine(sphere.position, start, end);
				auto distance = glm::length(linePoint - sphere.position);
				if (distance - sphere.radius <= 0) {
					positions.push_back(sphere.position);
				}
			}


			return positions;
		}

	private:
		struct Sphere {
			glm::vec3 position;
			float radius;
		};
		std::vector<Sphere> spheres;
		std::vector<uint32_t> freeList;
		std::vector<uint32_t> generations;

		bool _isValid(const Handle& handle) const {
			int32_t index = handle.getIndex();
			return index >= 0 && index < generations.size() && generations[index] == handle.getGeneration();
		}

		glm::vec3 _closestPointOnLine(const glm::vec3& point, const glm::vec3& start, const glm::vec3& end) {
			glm::vec3 lineDirection = glm::normalize(end - start);
			glm::vec3 qp = point - start;
			float scalarProjection = glm::dot(qp, lineDirection);
			scalarProjection = glm::clamp(scalarProjection, 0.0f, glm::length(end - start));
			glm::vec3 closestPoint = start + scalarProjection * lineDirection;
			return closestPoint;
		}
	};
}