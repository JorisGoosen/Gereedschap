#version 300 es

//in mediump vec2 tex;
in highp vec3 straal;
out mediump vec4 FragColor;

//uniform  float schermVerhouding;
uniform sampler2D landRuis;

void main()
{
	highp vec3 straalHier = normalize(straal - vec3(0., 0.6, 0.));
	const highp float stapje = 0.02;
	
	const highp vec3 begin = vec3(0., 1.75, -2.);;
	highp vec3 positie = begin;
	highp float hoogte = 0.0;

	FragColor = vec4(0., 0., straal.y, 1.);

	highp float totaal = 0.0;
	const mediump float maxTotaal = 3.0;

	bool wasIn = false;
	while(totaal < maxTotaal)
	{
		if(positie.y < 0.0)
		{
			FragColor = vec4(0., 0.5*totaal/maxTotaal, 0., 1.);
			break;
		}
		
		//if(positie.x >= -0.5 && positie.x <= 0.5 && positie.z >= 0.0 && positie.z <= 1.0)
		{
			wasIn = true;

			//in plaatje, raken we iets?
			highp vec2 plek = positie.xz + vec2(0.5, 0.);
			mediump vec4 kleurHier = texture2D(landRuis, plek);

			hoogte = kleurHier.r * 0.1;

			if(hoogte > positie.y)
			{
			//	hoogte = positie.y;//mix(hoogte, positie.y, 0.5);
				//FragColor = vec4(positie.y * 5., positie.y - hoogte, hoogte - positie.y, 1.0);
				hoogte = hoogte * 3.0;
				FragColor = vec4(hoogte, 0.5*totaal/maxTotaal - hoogte, hoogte, 1.); //vec4(positie.y * 5., hoogte * 3., 0.0, 1.0);

				break;
			}
		}
		//else if(wasIn)
		//	break;

		totaal += stapje;

		positie = begin + (totaal * straalHier);
	};

	//FragColor = vec4(totaal);	
}