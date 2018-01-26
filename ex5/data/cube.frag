#version 400

in vec4 g_vertex;
in vec3 g_normal;

out vec4 out_color;

uniform vec3 lightsource;
uniform mat3 normalMatrix;
uniform vec4 color;

void main()
{
	// TODO: uncomment the following lines to
	// activate simple shading.
	//float lambert = clamp(dot(normalize(normalMatrix * normalize(g_normal)), normalize(lightsource - g_vertex.xyz)), 0.0, 1.0);
	//out_color = vec4((0.2 + 0.8 * lambert) * color.xyz, color.w);

	out_color = vec4(1.0, 0.0, 0.0, 1.0); // dummy color
}
