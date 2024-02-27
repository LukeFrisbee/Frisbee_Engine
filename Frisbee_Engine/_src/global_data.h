#pragma once

#include <string>

namespace fengine {
	// SINGLETON
	class GlobalData {
	public:
		// DATA
		int fps{};
		std::string swapPresetMode{};
		bool isWireFrame = false;
		// ====

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