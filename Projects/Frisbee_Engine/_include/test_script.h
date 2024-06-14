#pragma once

#include <iostream>
#include "script.h"

namespace fengine {
	class TestScript : public Script
	{
	public:
		void Update() override {
			std::cout << "test!\n";
		}
	};
}