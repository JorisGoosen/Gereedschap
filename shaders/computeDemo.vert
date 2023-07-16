#version 300 es


layout(location = 0) in mediump vec2 Plaats;
layout(location = 1) in mediump vec2 Kleur;


uniform mat4 modelView;
uniform mat4 projectie;


out mediump vec4 kleur;

void main()
{
	gl_Position = projectie * modelView *  vec4(Plaats, 0.0, 1.0);

	kleur = vec4(Kleur.x, 0, Kleur.y, 1.0);//mix(vec4(1.0), vec4(1, 0, 0, 1), PlaatsKleur.w) * PlaatsKleur.z;
}