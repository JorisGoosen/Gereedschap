#version 440

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 vTex;

out vec2 tex;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	tex = vTex;
}