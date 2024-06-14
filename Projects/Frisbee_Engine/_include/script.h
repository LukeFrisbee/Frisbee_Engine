#pragma once

#include "input.h"
#include "screen.h"
#include "camera.h"
#include "rope_render_object_holder.h"
#include "model_builder.h"

namespace fengine {
	class Script {
	public:
		Script(Input& input, Screen& screen, Camera& camera, RenderObjectHolder& renderObjectHolder, ModelBuilder& modelBuilder)
			: s_input(input), s_screen(screen), s_camera(camera), s_renderObjectHolder(renderObjectHolder), s_modelBuilder(modelBuilder) {};
		virtual void Start() {};
		virtual void Update() {};
	protected:
		Input& s_input;
		Screen& s_screen;
		Camera& s_camera;
		RenderObjectHolder& s_renderObjectHolder;
		ModelBuilder& s_modelBuilder;
	};
}