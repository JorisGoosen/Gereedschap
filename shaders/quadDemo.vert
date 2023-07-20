#version 300 es

in mediump vec2 vPos;
in mediump vec2 vTex;

out mediump vec2 tex;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	tex = vTex;
}
