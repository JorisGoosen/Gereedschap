#version 300 es

in mediump vec2 tex;
out mediump vec4 FragColor;

uniform sampler2D landRuis;

void main(){
	FragColor = texture2D(landRuis, tex);
}