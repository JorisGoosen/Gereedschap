#version 400

out vec4 kleur;

in vec2 textuur;
in float hoogte;

uniform sampler2D handLand;
//uniform int vermindering;

void main()
{
	//vec3  tsjakka = normalize(vec3(dFdx(hoogte) * 500., dFdy(hoogte) * 500., 1.));
	const float hH = 0.05;
	//vec4 kleur = texture(handLand, textuur);
	float h = texture(handLand, textuur).r, //kleur.r,
				l = texture(handLand, textuur).b,// = 1.,//kleur.b,// = dot(vec3(0., 0., 1.), tsjakka),
				r = 1.-texture(handLand, textuur).g,//1.-kleur.g,
				w = h < hH ? 1.0 - h/hH : 0.;

	vec2 texG = vec2(textureSize(handLand, 0));

	vec2 invT = vec2(1.) / texG;

	//float verminderingF = float(vermindering);

	float xAfg = texture(handLand, textuur + vec2(invT.x, .0)).r - texture(handLand, textuur - vec2(invT.x, .0)).r,
				yAfg = texture(handLand, textuur + vec2(.0, invT.y)).r - texture(handLand, textuur - vec2(.0, invT.y)).r;

	vec3  tsjakka = normalize(vec3(xAfg * 40., 1., yAfg * 40.));

	l *= 0.3 + 0.7 * dot(normalize(vec3(-1., 1., 0.5)), tsjakka);

	//r = clamp((max(abs(xAfg), abs(yAfg)) * 160.), 0., 1.);

	const vec4 	water 	= vec4(0.0,  0.0, 0.5, 1.),
						rots	= vec4(0.6,  0.5, 0.4, 1.),
						plant	= vec4(0.05, 0.6, 0.0, 1.);

	kleur = l * mix(mix(plant, rots, r), water, w); 
}