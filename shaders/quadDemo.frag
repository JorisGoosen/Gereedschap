#version 310 es

in mediump vec2 tex;
out mediump vec4 FragColor;

void main(){
	FragColor = vec4(vec2(1.) + (2. * sin(tex * 33.)), 1. + (2. * cos(tex.y * tex.x * 133.7)), 1.);
}