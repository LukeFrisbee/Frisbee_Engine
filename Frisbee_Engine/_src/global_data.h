#pragma once

#include <string>
#include "glm/glm.hpp"

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
		std::string swapPresetMode{};
		bool isWireFrame = false;
		glm::vec3 camPos{};
		glm::vec3 camRot{};

		std::string debugString{};

		// PBR
		glm::vec3 albedo{ 1.0, 0.0, 0.0 };
		float metallic = 0.0;
		float roughness = 0.3;
		float ao = 0.02;

		// LineCast
		glm::vec3 lineStart{ 0.0 };
		glm::vec3 lineEnd{ 1.0 };

		static GlobalData& getInstance()
		{
			static GlobalData instance; // Guaranteed to be destroyed.
			// Instantiated on first use.
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