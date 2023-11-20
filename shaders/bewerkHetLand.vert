#version 420

in vec2 vPos;
in vec2 vTex;

//out vec2 tex;
out vec2 pixelPlek;

uniform float schermVerhouding;
uniform sampler2D landRuis;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	//tex = vTex;

	

	pixelPlek =  vec2(0.5) + (0.5 * vTex);
	//pixelPlek = ivec2(int((float(textuurBreedte) * (0.5 + 0.5 * vTex.x))), int((float(textuurHoogte) * (0.5 + 0.5 * vTex.y))));
}
