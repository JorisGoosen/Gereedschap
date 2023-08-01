#version 300 es

layout(location = 0) in highp vec3 vPos;
layout(location = 1) in highp vec2 vTex;

uniform mat4 projectie;
uniform mat4 modelView;

out highp vec2 textuur;

uniform sampler2D handLand;

void main()
{
	textuur =  vTex;// (vTex + vec2(1.0)) / 2.0;

	highp vec4 pos = vec4(vPos, 1.0);
	pos.y = texture(handLand, textuur).r;

	gl_Position = projectie * modelView * pos;
}