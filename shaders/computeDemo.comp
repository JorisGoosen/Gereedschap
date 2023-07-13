#version 300 es

layout(triangles) in;
layout(triangle_strip, max_vertices=64)

in mediump vec3 normal[];

uniform mat4 projectie;
uniform mat4 modelView;

out mediump vec3 normal;


void main()
{
	for(int i=0; i<3; i++)
	{
		gl_Position =// projectie * 
		//mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
		projectie * 
		//mat4(1) * 
		modelView * 
		gl_in[i].gl_Position;// * modelView;// * ;

		normal = normalize(gl_in[i].gl_Position.xyz);
		EmitVertex();
	}
}