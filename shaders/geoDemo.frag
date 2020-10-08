#version 440

out vec4 kleur;

in NaarFrag
{
	in vec3 normal;
	in vec2 tex;
	in vec4 kleur;
} fs_in;

void main()
{
	kleur = fs_in.kleur;
}
	