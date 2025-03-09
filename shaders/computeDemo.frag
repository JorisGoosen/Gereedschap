#version 440

out vec4 col;

in vec4 kleur;

uniform bool jeBentWolf;

void main()
{
	col =  kleur;
}