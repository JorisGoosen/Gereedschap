#version 120

varying vec2 tex;

void main(){
	gl_FragColor = vec4(vec2(1) + (2 * sin(tex * 33)), 1 + (2 * cos(tex.y * tex.x * 133.7)), 1);
}