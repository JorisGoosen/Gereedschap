#version 300 es

in mediump vec2 tex;
in mediump vec3 straal;
out mediump vec4 FragColor;

uniform sampler2D landRuis;

void main()
{
	const mediump float stapje = 0.01;
	
	const mediump vec3 begin = vec3(0., 0., -1.);;
	mediump vec3 positie = begin;
	mediump float hoogte = 0.0;

	FragColor = vec4(0.);

	mediump float totaal = 0.0;

	while(length(positie) < 3.0)
	{
		if(positie.x >= -0.5 && positie.x <= 0.5 && positie.z >= 0.0 && positie.z <= 1.0)
		{
			//in plaatje, raken we iets?
			hoogte = texture2D(landRuis, positie.xz + vec2(0.5, 0.)).r;

			if(hoogte > positie.y  + 0.5)
			{
				FragColor = vec4(vec3(hoogte), 1.0);

				positie = vec3(1235);
			}
		}

		totaal += stapje;

		positie = begin + totaal * straal;
	};

	//FragColor = vec4(totaal);	
}