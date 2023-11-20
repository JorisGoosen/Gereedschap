#version 400

in vec2 vPos;
in vec2 vTex;

out vec2 tex;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	tex = vTex;
}
