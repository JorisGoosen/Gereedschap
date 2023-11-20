#version 400

//in vec2 tex;
in vec2 pixelPlek;
out vec4 FragColor;

uniform sampler2D landTwee;

void main()
{
	ivec2 textuurGrootte = textureSize(landTwee, 0);
	ivec2 texelPos = ivec2(vec2(textuurGrootte) * pixelPlek);
	FragColor =  texelFetch(landTwee, texelPos, 0);
}