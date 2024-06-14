#include "physics/ray_cast.h"

#include "physics/physics_pool.h"

namespace fengine {
	bool RayCast::castLine(PhysicsShape* physicsShape, const glm::vec3& start, const glm::vec3& end)
	{
		auto& physicsShapes = PhysicsPool::getinstance().PhysicsShapes;

		glm::vec3 lineVec = end - start;

		for (auto* shape : physicsShapes) 
		{
			glm::vec3 closestPoint = _closestPointOnLine(shape->getPosition(), start, end);

			if (shape->collisionAtPos(closestPoint)) {
				physicsShape = shape;
				return true;
			}
		}

		return false;
	}

	glm::vec3 RayCast::_closestPointOnLine(const glm::vec3& point, const glm::vec3& start, const glm::vec3& end) 
	{
		glm::vec3 lineDirection = glm::normalize(end - start);
		glm::vec3 qp = point - start;
		float scalarProjection = glm::dot(qp, lineDirection);
		scalarProjection = glm::clamp(scalarProjection, 0.0f, glm::length(end - start));
		glm::vec3 closestPoint = start + scalarProjection * lineDirection;

		return closestPoint;
	}
}