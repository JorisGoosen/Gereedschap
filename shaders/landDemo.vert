#version 420

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTex;

uniform mat4 projectie;
uniform mat4 modelView;
uniform int vermindering;

out vec2 textuur;
out float hoogte;

uniform sampler2D handLand;

void main()
{
	textuur =  vTex;// (vTex + vec2(1.0)) / 2.0;

	vec4 pos = vec4(vPos, 1.0);
	//pos.y = textureLod(handLand, textuur, float(vermindering)).r * 0.5;
	pos.y = texture(handLand, textuur).r*0.5;

	gl_Position = projectie * modelView * pos;

	hoogte = pos.y;
}