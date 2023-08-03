#version 300 es

out mediump vec4 kleur;

in highp vec2 textuur;
in highp float hoogte;

uniform sampler2D handLand;
//uniform int vermindering;

void main()
{
	//highp vec3  tsjakka = normalize(vec3(dFdx(hoogte) * 500., dFdy(hoogte) * 500., 1.));
	const highp float hH = 0.005;
	highp float h = texture(handLand, textuur).r,
				l,// = dot(vec3(0., 0., 1.), tsjakka),
				r = h,
				w = h < hH ? 1.0 - h/hH : 0.;

	highp vec2 texG = vec2(textureSize(handLand, 0));

	highp vec2 invT = vec2(1.) / texG;

	//mediump float verminderingF = float(vermindering);

	highp float xAfg = texture(handLand, textuur + vec2(invT.x, .0)).r - texture(handLand, textuur - vec2(invT.x, .0)).r,
				yAfg = texture(handLand, textuur + vec2(.0, invT.y)).r - texture(handLand, textuur - vec2(.0, invT.y)).r;

	highp vec3  tsjakka = normalize(vec3(xAfg * 40., yAfg * 40., 1.));

	l = 0.3 + 0.7 * dot(normalize(vec3(1., 1., 1.)), tsjakka),

	r = clamp((max(abs(xAfg), abs(yAfg)) * 160.), 0., 1.);

	const mediump vec4 	water 	= vec4(0.0,  0.0, 0.5, 1.),
						rots	= vec4(0.6,  0.5, 0.4, 1.),
						plant	= vec4(0.05, 0.6, 0.0, 1.);

	kleur = l * mix(mix(plant, rots, r), water, w); 
}