#version 440

layout(location = 0) in vec4 PlaatsKleur;
//layout(location = 1) in vec4 HeHoHoLe;


uniform mat4 modelView;
uniform mat4 projectie;

uniform bool jeBentWolf;

out vec4 kleur;

void main()
{
	gl_Position = projectie * modelView *  vec4(PlaatsKleur.xy, 0.0, 1.0);
	gl_PointSize = max(1.0,  10.0 - 2.0*gl_Position.z);

	if(!jeBentWolf)
	{
		//vec2 heho = (vec2(1.0) + HeHoHoLe.xy) / 2.0;
		kleur = vec4(1.0);//heho.x, 1.0, heho.y, 1.0);
	}
	else
	{
		kleur = vec4(PlaatsKleur.z, 0.0, 0.0, 1.0);
	}
	// vec4(PlaatsKleur.w, 1.0, PlaatsKleur.z, 1.0);//mix(vec4(1.0), vec4(1, 0, 0, 1), PlaatsKleur.w) * PlaatsKleur.z;
}