#version 400

out vec4 uitKleur;

in vec3 normal;
in vec2 texU;
in vec4 kleur;


void main()
{
	uitKleur = kleur;
}
	