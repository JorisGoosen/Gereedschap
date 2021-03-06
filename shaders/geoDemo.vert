#version 440

layout(location = 0) in vec3	pos;
layout(location = 1) in vec2	tex;

uniform mat4 modelView;
uniform mat4 projectie;

out NaarFrag
{
	out vec3 normal;
	out vec2 tex;
	out vec4 kleur;
} tc_in;

void main()
{
	const vec3 mults = vec3(33, 61, 12);

	tc_in.normal	= normalize(pos);
	tc_in.tex		= tex;
	tc_in.kleur		= vec4(sin(tex.x * 3.142 * mults.x), sin(tex.y * 3.142 * mults.y), sin((tex.x + tex.y) * 3.142 * mults.z), 1) ;
	gl_Position		= projectie * modelView * vec4(pos, 1);	
}