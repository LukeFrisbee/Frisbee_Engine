#pragma once

#include "renderer_resources.h"
#include "model_builder.h"
#include "f_imgui_draw.h"


namespace fengine {
	class ResourceLoader {
	public:
		ResourceLoader(RendererResources& rendererResources, ModelBuilder& modelBuilder, EditorUI& editorUI);
	private:
		RendererResources& m_rendererResources;
		ModelBuilder& m_modelBuilder;
	};
}