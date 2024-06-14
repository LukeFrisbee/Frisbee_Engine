#include "rope_connector.h"
#include "global_data.h"
#include "camera.h";

#include "GLFW/glfw3.h"

#include "physics/ray_cast.h"

namespace fengine {
	void RopeConnector::Start() 
	{

	}

	void RopeConnector::Update() 
	{
		GlobalData& globalData = GlobalData::getInstance();

		auto camPos = s_camera.getPosition();

		double xPos;
		double yPos;
		s_input.getMousePos(xPos, yPos);

		auto mouseWorldPos = s_camera.getMouseWorldPosition(xPos, yPos, s_screen.getWidth(), s_screen.getHeight());

		globalData.mousePos = mouseWorldPos;
		auto direction = glm::normalize(mouseWorldPos - camPos);
		glm::vec3 rayEnd = camPos + direction * 100.0f;

		float planeZ = 0.0f;
		float t = (planeZ - mouseWorldPos.z) / direction.z;
		// Intersects Plane
		if (t >= 0) {
			glm::vec3 intersectionPoint = mouseWorldPos + t * direction;
			if (s_input.getMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				globalData.debugString += "Click!\n";

				RenderObject rope = RenderObject();
				Model::Data data{};
				data.vertices =
				{
					{ { 0.0, 1.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
					{ { 1.0, 1.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
					{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
					{ { 1.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
					{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
					{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} }
				};
				rope.model = s_modelBuilder.BuildModel(data);
				
				s_renderObjectHolder.addRenderObject(std::move(rope));
				s_renderObjectHolder.removeRenderObject(0);
			}
		}

		RayCast raycast{};
		PhysicsShape* shape = nullptr;
		if (raycast.castLine(shape, camPos, rayEnd))
		{
			globalData.debugString += "found shape!\n";
		}

		//auto isColliding = physicsSphere.collisionAtPos(cameraObject.transform.translation);
		//globalData.debugString += std::to_string(isColliding) + '\n';
	}
}