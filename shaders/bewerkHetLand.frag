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

mediump ivec2 textuurGrootte;

int vorigeX = -1, vorigeY = -1;
highp float vorigeH = -1.;

highp float texelHoogte(int x, int y)
{
	x= clamp(x, 0, textuurGrootte.x);
	y =  clamp(y, 0, textuurGrootte.y);
	/*if(x == vorigeX && y == vorigeY)
		return vorigeH;

	vorigeX = x;
	vorigeY = y;

	vorigeH = */
	
	return texelFetch(landRuis, ivec2(x,y), 0).r;

	//return vorigeH;
}

void main()
{
	//FragColor =  vec4(texelFetch(landRuis, pixelPlek, 0).r);
	//FragColor = texture(landRuis, pixelPlek);

	textuurGrootte =  textureSize(landRuis, 0);
	mediump ivec2 texelPos = ivec2(vec2(textuurGrootte) * pixelPlek);

	for(int x=-1;x<2;x++)
		for(int y=-1;y<2;y++)
			if(!(x==y && (x == -1 || x == 1)))
				hoogtes[(x+1)*3 + y+1] = texelHoogte(texelPos.x + x, texelPos.y + y); 

	//highp float hoogte = haalHoogte(1, 1);

	highp vec4 gradienten = abs(vec4(
		haalHoogte(0, 1) - haalHoogte(1, 1), 
		haalHoogte(2, 1) - haalHoogte(1, 1), 
		haalHoogte(1, 0) - haalHoogte(1, 1), 
		haalHoogte(1, 2) - haalHoogte(1, 1)));

	highp float maxGrad = max(max(gradienten.x, gradienten.y), max(gradienten.z, gradienten.w));


	highp float groente = clamp(1.0 - (maxGrad * 120.), 0., 1.);

	// nu gaan we licht bakken?
	// stel de zon komt schuin van een kant
	const highp float zonHoekPerStapje = 0.003;
		highp float		huidigeZonHoogte = haalHoogte(1,1),
				volgendeZonHoogte,
				zonLicht = 1.;

	bool doorGaan = true;

	const mediump vec2 zonStraal = vec2(1., -0.5);

	for(mediump vec2 zonPos = vec2(texelPos); 
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
		

	FragColor = vec4(haalHoogte(1,1), groente, zonLicht, mix(mix(gradienten.x, gradienten.y, 0.5), mix(gradienten.z, gradienten.w, 0.5), 0.5));//vec4(hoogte);
}