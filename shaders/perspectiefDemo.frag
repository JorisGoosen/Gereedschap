#version 440

out vec4 col;

in vec3 normal;

void main()
{
	col = vec4((vec3(1) + normal) / vec3(2), 1);
}