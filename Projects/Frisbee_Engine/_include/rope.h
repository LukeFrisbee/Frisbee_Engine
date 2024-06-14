#pragma once
#include "render_object.h"

namespace fengine {
	class Rope : public RenderObject
	{
	public:
		void hanldeRope(glm::vec3 start, glm::vec3 end);
	};
}