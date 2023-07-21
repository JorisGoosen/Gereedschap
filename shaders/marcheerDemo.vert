#version 300 es

in mediump vec2 vPos;
in mediump vec2 vTex;

out mediump vec2 tex;
out mediump vec3 straal;

uniform float schermVerhouding;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	tex = vTex;

	straal = normalize(vec3(tex.x * schermVerhouding, tex.y, 3.));
}
