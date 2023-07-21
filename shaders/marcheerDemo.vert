#version 300 es

in mediump vec2 vPos;
in highp vec2 vTex;

//out mediump vec2 tex;
out highp vec3 straal;

uniform float schermVerhouding;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	//tex = vTex;

	straal = vec3(vTex.x * schermVerhouding * 0.25, vTex.y * 0.25, 1.0);
}
