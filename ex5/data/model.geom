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

mat4 RotationMatrix(vec3 axis, float angle)
{
axis = normalize(axis);
float s = sin(angle);
float c = cos(angle);
float oc = 1.0 - c;

return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
            oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
            oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
            0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
	for (int i = 0; i < 3; ++i)
	{
		mat4 localTransform = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

		normal = normalize(geom_normal[i]);
		vertex = localTransform * gl_in[i].gl_Position;
		gl_Position = viewprojection * vertex;

		mat4 rot = RotationMatrix(vec3(0.0f,1.0f,0.0f),geom_normal[i].x * geom_normal[i].z);
		mat4 trans = TranslateMatrix(gPosition[0]);
		mat4 m = trans * rot;


		EmitVertex();
	}

	EndPrimitive();
}
