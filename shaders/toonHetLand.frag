#version 300 es

//in mediump vec2 tex;
in highp vec2 pixelPlek;
out mediump vec4 FragColor;

uniform sampler2D landRuis;

void main()
{
	highp ivec2 textuurGrootte = textureSize(landRuis, 0);
	highp ivec2 texelPos = ivec2(vec2(textuurGrootte) * pixelPlek);
	FragColor =  texelFetch(landRuis, texelPos, 0);
}