#version 400

layout (location = 0) in vec3 in_vertex;

out vec3 v_vertex;

void main()
{
	v_vertex = in_vertex;

	// TODO: Comment out this line when the geometry shaders is used again.
	gl_Position = vec4(v_vertex, 1.0);
}
