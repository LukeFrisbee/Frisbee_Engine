#include "rope.h"

#include <vector>

#include "glm/glm.hpp"

namespace fengine {
	void Rope::hanldeRope( glm::vec3 start, glm::vec3 end) {
		float s = 0.75f / 2;

		glm::vec3 direction = glm::normalize(end - start);
		glm::vec3 perpendicular = glm::vec3(direction.y, -direction.x, 0);

		glm::vec3 flu = start + (s * -perpendicular);
		glm::vec3 fld = start + (s * perpendicular);
		glm::vec3 fru = end + (s * -perpendicular);
		glm::vec3 frd = end + (s * perpendicular);
		glm::vec3 blu = start + (s * -perpendicular);
		glm::vec3 bld = start + (s * perpendicular);
		glm::vec3 bru = end + (s * -perpendicular);
		glm::vec3 brd = end + (s * perpendicular);

		std::vector<glm::vec3> vertices = {
			fld, flu, frd, flu, fru, frd
			//brd, blu, bld, brd, bru, blu
		};

		std::vector<Model::Vertex> vertexData =
		{
			{ fld, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ flu, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ frd, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ flu, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ fru, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ frd, {0.0, 0.0, 1.0}, {0.0, 0.0} },
		};

		model->updateVertexBuffers(vertexData);

		//auto normals = glm::vec3[]{
		//Vector3.BACK, Vector3.BACK, Vector3.BACK, Vector3.BACK, Vector3.BACK, Vector3.BACK
		//			#Vector3.FORWARD, Vector3.FORWARD, Vector3.FORWARD, Vector3.FORWARD, Vector3.FORWARD, Vector3.FORWARD
		//}
		// 
		//
		//auto surface_array = []
		//surface_array.resize(Mesh.ARRAY_MAX)
		//surface_array[Mesh.ARRAY_VERTEX] = vertices
		//surface_array[Mesh.ARRAY_NORMAL] = normals
		//#surface_array[Mesh.ARRAY_INDEX] = indices
		//
		//# Create the Mesh.
		//auto arr_mesh = ArrayMesh.new()
		//arr_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, surface_array)
		//yarn.mesh = arr_mesh
		//yarn.collision_shape.shape = arr_mesh.create_trimesh_shape()
		//#yarn.material_override = blue_mat
	}
}