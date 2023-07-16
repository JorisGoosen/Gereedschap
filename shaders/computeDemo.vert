#version 300 es

layout(location = 0) in mediump vec4 PlaatsKleur;

out mediump vec4 kleur;

void main()
{
	gl_Position = vec4(0,0,0,1);// vec4(PlaatsKleur.xy, 0.0, 1.0);

	kleur = vec4(1); // mix(vec4(1.0), vec4(1, 0, 0, 1), PlaatsKleur.w) * PlaatsKleur.z;
}