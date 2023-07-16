#version 300 es


layout(location = 0) in mediump vec4 PlaatsKleur;


uniform mat4 modelView;
uniform mat4 projectie;


out mediump vec4 kleur;

void main()
{
	gl_Position = projectie * modelView *  vec4(PlaatsKleur.xy, 0.0, 1.0);

	kleur = vec4(PlaatsKleur.w, 0, PlaatsKleur.z, 1.0);//mix(vec4(1.0), vec4(1, 0, 0, 1), PlaatsKleur.w) * PlaatsKleur.z;
}