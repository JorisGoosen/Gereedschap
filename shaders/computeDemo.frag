#version 300 es

out mediump vec4 col;

in mediump vec4 kleur;

void main()
{
	col = kleur;
}