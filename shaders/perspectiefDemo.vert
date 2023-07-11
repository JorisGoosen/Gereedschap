#version 300 es

layout(location = 0) in mediump vec3 vPos;

uniform mat4 projectie;
uniform mat4 modelView;

out mediump vec3 normal;

void main()
{
	normal = normalize(vPos);

	gl_Position =// projectie * 
	//mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
	projectie * 
	//mat4(1) * 
	modelView * 
	vec4(vPos, 1.0);// * modelView;// * ;
}