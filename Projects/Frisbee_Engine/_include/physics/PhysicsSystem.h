#pragma once

#include "logger.h"
#include "handle.h"
#include "entt/entt.hpp"

#include "components/transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include <glm/gtx/norm.hpp>

namespace physics {
	class PhysicsSystem {
	public:
		PhysicsSystem(entt::registry& registry) : m_registry{ registry } {};

		std::vector<entt::entity> lineCast(const glm::vec3& start, const glm::vec3& end) {
			std::vector<entt::entity> collisions;

			auto sphereView = m_registry.view<fengine::PhysicsSphere, fengine::Transform>();
			for (auto& entity : sphereView) {
				auto position = sphereView.get<fengine::Transform>(entity).position;
				auto radius = sphereView.get<fengine::PhysicsSphere>(entity).radius;

				auto linePoint = _closestPointOnLine(position, start, end);
				auto distance = glm::length(linePoint - position);
				if (distance - radius <= 0) {
					collisions.push_back(entity);
				}
			}
			auto capsuleView = m_registry.view<fengine::PhysicsCapsule, fengine::Transform>();
			for (auto& entity : capsuleView) {
				auto position = capsuleView.get<fengine::Transform>(entity).position;
				auto capsule = capsuleView.get<fengine::PhysicsCapsule>(entity);

				auto isCollision = _lineCapsuleCollision(start, end,
					position, position + capsule.offset, capsule.radius);
				if (isCollision) {
					collisions.push_back(entity);
					LOG_DEBUG("collision!");
				}
			}

			return collisions;
		}

	private:
		entt::registry& m_registry;

		bool _lineCapsuleCollision(const glm::vec3& lineStart, const glm::vec3& lineEnd,
			const glm::vec3& capsuleStart, const glm::vec3& capsuleEnd, const float& capsuleRadius) {
			glm::vec3 closest1, closest2;
			_closestPointsBetweenSegments(lineStart, lineEnd, capsuleStart, capsuleEnd, closest1, closest2);
			float distance = glm::distance(closest1, closest2);
			return distance <= capsuleRadius;
		}

		void _closestPointsBetweenSegments(const glm::vec3& p1, const glm::vec3& q1,
			const glm::vec3& p2, const glm::vec3& q2,
			glm::vec3& closest1, glm::vec3& closest2) {
			glm::vec3 d1 = q1 - p1;
			glm::vec3 d2 = q2 - p2;
			glm::vec3 r = p1 - p2;
			float a = glm::dot(d1, d1);
			float e = glm::dot(d2, d2);
			float f = glm::dot(d2, r);

			// Check if either segment is a point
			if (glm::epsilonEqual(a, 0.0f, glm::epsilon<float>()) && glm::epsilonEqual(e, 0.0f, glm::epsilon<float>())) {
				// Both segments are points
				closest1 = p1;
				closest2 = p2;
				return;
			}
			if (glm::epsilonEqual(a, 0.0f, glm::epsilon<float>())) {
				// First segment is a point
				closest1 = p1;
				closest2 = p2 + (f / e) * d2;
				return;
			}
			if (glm::epsilonEqual(e, 0.0f, glm::epsilon<float>())) {
				// Second segment is a point
				closest1 = p1 + (glm::dot(d1, r) / a) * d1;
				closest2 = p2;
				return;
			}

			float c = glm::dot(d1, r);
			float b = glm::dot(d1, d2);
			float denom = a * e - b * b;

			float s, t;
			if (glm::epsilonNotEqual(denom, 0.0f, glm::epsilon<float>())) {
				s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else {
				s = 0.0f;
			}

			t = glm::clamp((b * s + f) / e, 0.0f, 1.0f);

			// Recompute s based on the new value of t
			s = glm::clamp((b * t - c) / a, 0.0f, 1.0f);

			closest1 = p1 + s * d1;
			closest2 = p2 + t * d2;
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