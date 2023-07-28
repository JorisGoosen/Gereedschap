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

highp ivec2 textuurGrootte;

highp float texelHoogte(int x, int y)
{
	return texelFetch(landRuis, ivec2(clamp(x, 0, textuurGrootte.x), clamp(y, 0, textuurGrootte.y)), 0).r;
}

void main()
{
	//FragColor =  vec4(texelFetch(landRuis, pixelPlek, 0).r);
	//FragColor = texture(landRuis, pixelPlek);

	textuurGrootte =  textureSize(landRuis, 0);
	highp ivec2 texelPos = ivec2(vec2(textuurGrootte) * pixelPlek);

	for(int x=-1;x<2;x++)
		for(int y=-1;y<2;y++)
			hoogtes[(x+1)*3 + y+1] = texelHoogte(texelPos.x + x, texelPos.y + y); 

	highp float hoogte = haalHoogte(1, 1);

	highp vec4 gradienten = abs(vec4(
		haalHoogte(0, 1) - hoogte, 
		haalHoogte(2, 1) - hoogte, 
		haalHoogte(1, 0) - hoogte, 
		haalHoogte(1, 2) - hoogte));

	highp float maxGrad = max(max(gradienten.x, gradienten.y), max(gradienten.z, gradienten.w));


	highp float groente = clamp(1.0 - (maxGrad * 100.), 0., 1.);

	// nu gaan we licht bakken?
	// stel de zon komt schuin van een kant
	highp float zonHoekPerStapje = 0.003,
				huidigeZonHoogte = hoogte,
				volgendeZonHoogte,
				zonLicht = 1.;

	bool doorGaan = true;

	highp vec2 zonStraal = vec2(1., -0.5);

	for(highp vec2 zonPos = vec2(texelPos); 
		doorGaan && zonPos.x >= 0. && zonPos.y >= 0. && zonPos.x < float(textuurGrootte.x) && zonPos.y < float(textuurGrootte.y);
		zonPos += zonStraal
		)
	
		{
			huidigeZonHoogte += zonHoekPerStapje;
			volgendeZonHoogte = texelHoogte(int(ceil(zonPos.x)), int(ceil(zonPos.y)));

			if(volgendeZonHoogte >= huidigeZonHoogte)
			{
				zonLicht = 0.5;
				doorGaan = false;
			}
			
			if(huidigeZonHoogte > 1.0)
				doorGaan = false;
		}
		

	FragColor = vec4(hoogte, groente, zonLicht, mix(mix(gradienten.x, gradienten.y, 0.5), mix(gradienten.z, gradienten.w, 0.5), 0.5));//vec4(hoogte);
}