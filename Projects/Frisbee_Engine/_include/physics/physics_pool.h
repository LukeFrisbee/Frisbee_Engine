//#include <vector>
//
//#include "physics_shape.h"
//
//#pragma once
//class PhysicsPool {
//public:
//	std::vector<PhysicsShape*> PhysicsShapes;
//
//	static PhysicsPool& getinstance()
//	{
//		static PhysicsPool instance = PhysicsPool();
//		return instance;
//	}
//
//	PhysicsPool(const PhysicsPool&) = delete;
//	PhysicsPool(PhysicsPool&&) = delete;
//	PhysicsPool& operator=(const PhysicsPool&) = delete;
//	PhysicsPool& operator=(PhysicsPool&&) = delete;
//
//private:
//	PhysicsPool() {};
//};