#pragma once

#include <iostream>
#include "script.h"

#include "logger.h"
#include "entt/entt.hpp"
#include "components/transform.h"

namespace fengine {
	class TestScript : public Script
	{
	public:
		TestScript(entt::registry& ecs) : m_ecs(ecs) {}

		void transformUpdate(Transform& transform) {

		}

		void Start() override {
			//m_ecs.on_update<Transform>.connect<&transformUpdate>();

			auto entity = m_ecs.create();
			m_ecs.emplace<Name>(entity, "Jeep");
			m_ecs.emplace<Transform>(entity);
			m_ecs.emplace<RenderId>(entity, 0u);

			for (int i = 0; i < 10; i++) {
				auto entity = m_ecs.create();
				m_ecs.emplace<Name>(entity, "test" + std::to_string(i));
				m_ecs.emplace<Transform>(entity);
			}
		}

		void Update() override {

		}

	private:
		entt::registry& m_ecs;
	};
}