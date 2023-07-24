#version 300 es

in mediump vec2 vPos;
in highp vec2 vTex;

//out mediump vec2 tex;
out highp vec3 pixelPlek;

uniform float schermVerhouding;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	//tex = vTex;

	pixelPlek = vec3(vTex.x, (vTex.y / schermVerhouding) + 0.5, -1.0);
}
