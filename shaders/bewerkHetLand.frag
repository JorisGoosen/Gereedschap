#version 300 es

//in mediump vec2 tex;
in highp vec2 pixelPlek;
out mediump vec4 FragColor;

//uniform  float schermVerhouding;
uniform sampler2D landRuis;

highp float hoogtes[9];

highp float haalHoogte(int x, int y)
{
	return hoogtes[x*3 + y];
}

void main()
{
	//FragColor =  vec4(texelFetch(landRuis, pixelPlek, 0).r);
	//FragColor = texture(landRuis, pixelPlek);

	highp ivec2 textuurGrootte = textureSize(landRuis, 0);
	highp ivec2 texelPos = ivec2(vec2(textuurGrootte) * pixelPlek);

	for(int x=-1;x<2;x++)
		for(int y=-1;y<2;y++)
			hoogtes[(x+1)*3 + y+1] = texelFetch(landRuis, ivec2(clamp(texelPos.x + x, 0, textuurGrootte.x), clamp(texelPos.y + y, 0, textuurGrootte.y)), 0).r; 

	highp float hoogte = haalHoogte(1, 1);

	highp vec4 gradienten = abs(vec4(
		haalHoogte(0, 1) - hoogte, 
		haalHoogte(2, 1) - hoogte, 
		haalHoogte(1, 0) - hoogte, 
		haalHoogte(1, 2) - hoogte));

	highp float maxGrad = max(max(gradienten.x, gradienten.y), max(gradienten.z, gradienten.w));



	FragColor = vec4(hoogte, clamp(1.0 - (maxGrad * 80.), mix(gradienten.x, gradienten.y, 0.5), mix(gradienten.z, gradienten.w, 0.5)), .0, 1.);//vec4(hoogte);
}