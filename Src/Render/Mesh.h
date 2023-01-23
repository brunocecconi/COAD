
#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include "Render/Common.h"

namespace Render
{

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;
};

struct Mvp
{
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

private:
	eastl::vector<Vertex> mVertices{DEBUG_NAME_VAL("Render::Mesh")};
};

}

#endif
