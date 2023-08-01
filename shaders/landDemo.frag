#version 300 es

out mediump vec4 kleur;

in highp vec2 textuur;

uniform sampler2D handLand;

void main()
{
	highp float h = texture(handLand, textuur).r;
	kleur = vec4(0., h, clamp(0.1 - h, 0., 0.1) * 5., 1.);
}