#version 300 es

//in mediump vec2 tex;
in highp vec3 pixelPlek;
out mediump vec4 FragColor;

//uniform  float schermVerhouding;
uniform sampler2D landTwee;
uniform highp mat4 modelZicht;
uniform highp mat4 inversie;

highp float dot2( in highp vec2 v ) { return dot(v,v); }
highp float dot2( in highp vec3 v ) { return dot(v,v); }
highp float ndot( in highp vec2 a, in highp vec2 b ) { return a.x*b.x - a.y*b.y; }


highp float sdBox( highp vec3 p, highp vec3 b )
{
  highp vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

void main()
{
	const highp vec3 oog = vec3(0., 1.0, -1.9);
	
	highp vec3 straal = normalize(pixelPlek - mat3(modelZicht) * oog);
	const highp float stapje = 0.0015;
	
	
	highp vec3 positie = pixelPlek;
	highp float hoogte = 0.0;

	highp float totaal = 0.0;
	const mediump float maxTotaal = 12.0;
	const mediump vec4 water = vec4(0.,0.,0.4, 1.0),
						plant = vec4(0.0, 0.5, 0., 1.),
						rots	= vec4(vec3(0.5), 1.),
						strand = vec4(0.5, 0.8, 0.2, 1.);

	highp float afstand, vorigeAfstand;
	const highp vec3 landDoos = vec3(.5, .5, .5);
	const highp float plantHoogteMod = 0.02;
	mediump vec4 landKleur, kleurHier;

	highp ivec2 plek, laatstePlek;
	highp vec2 fPlek, textuurGrootte = vec2(textureSize(landTwee, 0));

	while(totaal < maxTotaal)
	{
		vorigeAfstand = afstand;

		afstand = sdBox(positie + vec3(0., 0., .5), landDoos);

		if(afstand > vorigeAfstand && afstand > 0.5)
		{
			FragColor = water;//mix(water, vec4(0., 0., (straal.y), 1.), clamp(1. + positie.y * 0.5, 0., 1.));

			break;
		}

		//if(positie.x >= -0.5 && positie.x <= 0.5 && positie.z >= 0.0 && positie.z <= 1.0)
		if(afstand < stapje)
		{
			positie = positie;
			//in plaatje, raken we iets?
			fPlek = positie.xz + vec2(0.5, 1.);
			//fPlek = vec2(clamp(fPlek.x, 0., 1.), clamp(fPlek.y, 0., 1.));
			//plek = ivec2(textuurGrootte * fPlek);

			//if(plek != laatstePlek)
			kleurHier = texture(landTwee, fPlek);//texelFetch(landTwee, plek, 0);

			//laatstePlek = plek;

			hoogte = kleurHier.r + kleurHier.g * plantHoogteMod;

			if(hoogte * landDoos.y > positie.y)
			{
			//	hoogte = positie.y;//mix(hoogte, positie.y, 0.5);
				//FragColor = vec4(positie.y * 5., positie.y - hoogte, hoogte - positie.y, 1.0);
				//hoogte = hoogte * 2.0;
				//FragColor = vec4(hoogte, 1.0 - 0.5*totaal/maxTotaal - hoogte, hoogte, 1.); //vec4(positie.y * 5., hoogte * 3., 0.0, 1.0);
				const highp float waterHoogte = 0.025;
				if(hoogte < waterHoogte)
					landKleur = mix(water, plant, clamp(hoogte-kleurHier.g * plantHoogteMod, 0.0, waterHoogte) / waterHoogte);
				else if(hoogte < 0.5)
					landKleur = mix(rots, plant, kleurHier.g);
				else if(hoogte > 0.7)
					landKleur = mix(rots, vec4(1.0), kleurHier.g);
				else
					landKleur = rots;

				landKleur *= kleurHier.b;

				FragColor = landKleur;//mix(water, landKleur, clamp(hoogte - kleurHier.g * plantHoogteMod, 0., 0.01) * 100.);	 //mix(strand, landKleur, clamp(hoogte - 0.05, 0., .1) * 10.), clamp(hoogte, 0., 0.05) * 20.);
				//FragColor = kleurHier;
				//FragColor.x = fPlek.x;
				//FragColor.z = fPlek.y;

				break;
			}
		
			totaal += stapje;
		}
		else
			totaal += afstand;

		positie = pixelPlek + (totaal * straal);
	};

	//FragColor = vec4(totaal);	
}