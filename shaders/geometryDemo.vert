#version 300 es

layout(location = 0) in mediump vec3 vPos;

void main()
{
	gl_Position = vec4(vPos, 1.0);
}