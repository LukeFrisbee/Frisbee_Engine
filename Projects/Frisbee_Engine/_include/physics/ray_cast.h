//#pragma once
//
//#include <vector>
//
//#include "glm/glm.hpp"
//
//#include "physics_shape.h"
//
//namespace fengine {
//	class RayCast {
//	public:
//		std::vector<PhysicsShape*> castLine(const glm::vec3& start, const glm::vec3& end);
//		//bool castRay(PhysicsShape& physicsShape, const glm::vec3& startPosition, const glm::vec3& direction, float length);
//	private:
//		glm::vec3 _closestPointOnLine(const glm::vec3& point, const glm::vec3& start, const glm::vec3& end);
//	};
//}