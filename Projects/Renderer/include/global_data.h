#pragma once

#include <string>
#include "glm/glm.hpp"
#include <queue>

/*
TODO:
			Temporary Singleton for displaying Debug information
*/

namespace fengine {
	// SINGLETON
	class GlobalData {
	public:
		// DATA
		int fps{};
		std::queue<int> fpsQueue;

		std::string swapPresetMode{};
		bool isWireFrame = false;
		glm::vec3 camPos{};
		glm::vec3 camRot{};
		glm::vec3 lightPos{};
		glm::vec3 mousePos{};

		std::string debugString{};

		// PBR
		glm::vec3 albedo{ 1.0, 0.0, 0.0 };
		float metallic = 0.0f;
		float roughness = 0.3f;
		float ao = 0.02f;

		// LineCast
		glm::vec3 lineStart{ 0.0 };
		glm::vec3 lineEnd{ 1.0 };

		static GlobalData& getInstance()
		{
			static GlobalData instance;
			return instance;
		}



		GlobalData(const GlobalData&) = delete;
		GlobalData(GlobalData&&) = delete;
		GlobalData& operator=(const GlobalData&) = delete;
		GlobalData& operator=(GlobalData&&) = delete;

	private:
		GlobalData() {};
	};
}