#version 400

in vec2 vPos;
in vec2 vTex;

//out vec2 tex;
out vec3 pixelPlek;

uniform sampler2D landTwee;
uniform float schermVerhouding;
uniform mat4 modelZicht;

void main(){
	gl_Position = vec4(vPos, 0.0, 1.0);
  	//tex = vTex;

	pixelPlek = mat3(modelZicht) * vec3(vTex.x, (vTex.y / schermVerhouding) + 0.5, -1.0);
}
