#version 440

out vec4 col;

in vec4 kleur;

uniform bool jeBentWolf;

void main()
{
	col =  (jeBentWolf ? vec4(0.1) : vec4(1.0)) * kleur;
}