#version 300 es

out mediump vec4 uitKleur;

in mediump vec3 normal;
in mediump vec2 texU;
in mediump vec4 kleur;


void main()
{
	uitKleur = kleur;
}
	