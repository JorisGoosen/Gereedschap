#version 440

in vec2 tex;
out vec4 col;

void main(){
	col = vec4(vec2(1) + (2 * tex), 0, 1);
}