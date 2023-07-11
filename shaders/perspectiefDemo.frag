#version 300 es

out mediump vec4 col;

in mediump vec3 normal;

void main()
{
	col = vec4((vec3(1) + normal) / vec3(2), 1);
}