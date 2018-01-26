#version 150

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 viewprojection;
uniform float animationFrame;

in vec3 geom_normal[];

out vec4 vertex;
out vec3 normal;

/////////////////////////////////////////////////////////////////////////////////////////////////
// TODO:
// Note: In GLSL matrices are defined in column-major order.
// Take into account the animationFrame parameter:
//      0.0f: No transformation at all
//      1.0f: Maximum rotation of 360° and maximum translation
/////////////////////////////////////////////////////////////////////////////////////////////////

void main()
{
	for (int i = 0; i < 3; ++i)
	{
		mat4 localTransform = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

		normal = normalize(geom_normal[i]);
		vertex = localTransform * gl_in[i].gl_Position;
		gl_Position = viewprojection * vertex;

		EmitVertex();
	}

	EndPrimitive();
}
