#pragma once

#include "renderer_resources.h"
#include "f_imgui_draw.h"


namespace fengine {
	class ResourceLoader {
	public:
		ResourceLoader(RendererResources& rendererResources);
	private:
		RendererResources& m_rendererResources;
	};
}