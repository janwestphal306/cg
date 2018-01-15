#version 150
#define M_PI 3.1415926535897932384626433832795

uniform mat4 transform;
uniform mat4 viewprojection;
uniform vec3 overallObjectDimensions;
uniform float animationFrame;
uniform int globalDeformationMode;

in vec3 in_vertex;
in vec3 in_normal;

out vec3 normal;
out vec4 vertex;

vec3 mold(vec3 v, float moldPlateau)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO:
    // Apply a mold deformation to the given vertex v.
    // Take into account the moldPlateau parameter
    //      1.0f: No deformation at all
    //      0.0f: Maximum deformation
    // Tip: Use overallObjectDimensions to get the extents of the x, y and z dimension
    // Tip: Keep in mind that the box is located in the coordinate system origin
    /////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Abstand der Vertices von der xy-Ebene und Winkels zwischen dem jeweiligen Vertex (als Vektor interpretiert) und der z-Achse
	// shape function: 0 unchanged, 1 max change
	float s = (atan(abs(v.x),abs(v.z)) / M_PI) * 2;

	v.z *= 1.0 - (1.0 - moldPlateau) * s;

    return v;
}

vec3 pinch(vec3 v, float pinchPlateau)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // TODO:
    // Apply a pinch deformation to the given vertex v.
    // Take into account the pinchPlateau parameter
    //      1.0f: No deformation at all
    //      0.0f: Maximum deformation
    // Tip: Use overallObjectDimensions to get the extents of the x, y and z dimension
    // Tip: Keep in mind that the box is located in the coordinate system origin
    /////////////////////////////////////////////////////////////////////////////////////////////////
	
	// shape function: 0 unchanged, 1 max change
	// unchanged when v[1] = bottom and max change when v[1] = top
	float bottom = -overallObjectDimensions[1] / 2.0;
	float s = (v[1] - bottom) / overallObjectDimensions[1]; 

	v[0] *= 1.0 - (1.0 - pinchPlateau) * s;

	return v;
}

vec3 twist(vec3 v, float maxAngle)
{
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO:
    // Apply a twist deformation to the given vertex v.
    // Take into account the maxAngle parameter, that defines the maximum rotation angle
    // Tip: Use overallObjectDimensions to get the extents of the x, y and z dimension
    // Tip: Keep in mind that the box is located in the coordinate system origin
    /////////////////////////////////////////////////////////////////////////////////////////////////

	// shape function: 0 unchanged, 1 max change
	// unchanged when v[1] = bottom and max change when v[1] = top
	float bottom = -overallObjectDimensions[1] / 2.0;
	float s = (v[1] - bottom) / overallObjectDimensions[1];

	// Tmp = PI * Ampli * Shape (Point->z);
	float tmp = maxAngle * s;

	// Cos = cos (Tmp); Sin = sin (Tmp); Tmp = Point->x;
	float Cos = cos(tmp);
	float Sin = sin(tmp);
	tmp = v[0];

	// Point->x = Cos * Tmp - Sin * Point->y;
	v[0] = Cos * tmp - Sin * v[2];
	// Point->y = Sin * Tmp + Cos * Point->y;
	v[2] = Sin * tmp + Cos * v[2];
    return v;
}

vec3 bend(vec3 v, float maxAngle)
{
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO:
    // Apply a bend deformation to the given vertex v.
    // Take into account the maxAngle parameter, that defines the maximum rotation angle
    // Tip: Use overallObjectDimensions to get the extents of the x, y and z dimension
    // Tip: Keep in mind that the box is located in the coordinate system origin
    /////////////////////////////////////////////////////////////////////////////////////////////////
	// Tmp = PI * Ampli * Shape (Point->z);
	float bottom = -overallObjectDimensions[1] / 2.0;
	float s = (v[1] - bottom) / overallObjectDimensions[1]; 

	float Tmp = maxAngle * s;

	// Cos = cos (Tmp); Sin = sin (Tmp); Tmp = Point->z;
	float Cos = cos(Tmp);
	float Sin = sin(Tmp);
	Tmp = v[1];

	// Point->z = Cos * Tmp - Sin * Point->x;
	v[1] = Cos * Tmp - Sin * v[0];
	// Point->x = Sin * Tmp + Cos * Point->x;
	v[0] = Sin * Tmp + Cos * v[0];
    return v;
}

void main()
{
    vec3 transV;

    switch(globalDeformationMode)
    {
        case 1: transV = mold(in_vertex, 1.0f - animationFrame); break;
        case 2: transV = pinch(in_vertex, 1.0f - animationFrame); break;
        case 3: transV = twist(in_vertex, radians(animationFrame * 360.0f)); break;
        case 4: transV = bend(in_vertex, radians(animationFrame * 90.0f)); break;
        default: transV = in_vertex;
    }

    gl_Position = viewprojection * transform * vec4(transV, 1.0);
    vertex = transform * vec4(transV, 1.0);
    normal = in_normal;
}
