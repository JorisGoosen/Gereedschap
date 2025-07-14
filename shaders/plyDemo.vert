#version 440

layout(location = 0) in vec3 vPos;

uniform mat4 projectie;
uniform mat4 modelView;

out vec3 normal;

void main()
{
	normal = normalize(vPos);

	gl_Position =
	projectie * 
	modelView * 
	vec4(vPos, 1.0);
}