#pragma once

#include <memory>

#include "model.h"
#include "device.h"

namespace fengine {
	class ModelBuilder {
	public:
		ModelBuilder(Device& fDevice) : m_device{ fDevice } {};

		std::unique_ptr<Model> BuildModel(Model::Data data) { return std::make_unique<Model>(m_device, data); }
		std::unique_ptr<Model> BuildModelFromFile(const std::string& filePath);

		ModelBuilder(const ModelBuilder&) = delete;
		ModelBuilder& operator= (const ModelBuilder&) = delete;

	private:
		Device& m_device;
	};
}