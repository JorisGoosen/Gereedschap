#version 120

attribute vec2 vPos;
attribute vec2 vTex;

varying vec2 tex;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	tex = vTex;
}
