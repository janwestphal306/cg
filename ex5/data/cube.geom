#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 viewprojection;
uniform mat4 view;
uniform mat3 normalMatrix;

in vec3 te_vertex[];

out vec4 g_vertex;
out vec3 g_normal;

void main()
{
	// TODO: Calculate the per patch normal by using the input vertices te_vertex[].
	// Use the calculated and transformed normal for all vertices of the ouput triangle.
	vec3 A = te_vertex[2] - te_vertex[0];
    vec3 B = te_vertex[1] - te_vertex[0];
    g_normal = normalMatrix * normalize(-cross(A, B));

	g_vertex = view * vec4(te_vertex[0], 1.0);
	gl_Position = viewprojection * vec4(te_vertex[0], 1.0);
	//g_normal =  normalMatrix * te_vertex[0];
	EmitVertex();

	g_vertex = view * vec4(te_vertex[1], 1.0);
	gl_Position = viewprojection * vec4(te_vertex[1], 1.0);
	//g_normal =  normalMatrix * te_vertex[1];
	EmitVertex();

	g_vertex = view * vec4(te_vertex[2], 1.0);
	gl_Position = viewprojection * vec4(te_vertex[2], 1.0);
	//g_normal =  normalMatrix * te_vertex[2];
	EmitVertex();

	EndPrimitive();
}
