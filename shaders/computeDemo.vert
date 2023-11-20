#version 400


layout(location = 0) in vec4 PlaatsKleur;


uniform mat4 modelView;
uniform mat4 projectie;


out vec4 kleur;

void main()
{
	gl_Position = projectie * modelView *  vec4(PlaatsKleur.xy, 0.0, 1.0);
	gl_PointSize = max(1.0,  10.0 - 2.0*gl_Position.z);

	kleur = vec4(PlaatsKleur.w, 0, PlaatsKleur.z, 1.0);//mix(vec4(1.0), vec4(1, 0, 0, 1), PlaatsKleur.w) * PlaatsKleur.z;
}