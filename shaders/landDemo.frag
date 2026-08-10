//WGSL fragment-shader voor landDemo

struct BeeldParameters {
    schermBreedte : f32,
    schermHoogte : f32,
    schermVerhouding : f32,
    _opvulling : f32,
};

@group(0) @binding(0) var<uniform> beeld : BeeldParameters;

@group(1) @binding(0) var handLand : texture_2d<f32>;
@group(1) @binding(1) var handLandSampler : sampler;

struct FragIn {
    @location(0) textuur : vec2f,
    @location(1) hoogte : f32,
};

struct FragUit {
    @location(0) kleur : vec4f,
};

@fragment
fn main(in : FragIn) -> FragUit {
    var uit : FragUit;

    const hH = 0.05;

    let h = textureSample(handLand, handLandSampler, in.textuur).r;
    let l = textureSample(handLand, handLandSampler, in.textuur).b;
    let r = 1.0 - textureSample(handLand, handLandSampler, in.textuur).g;
    let w = select(0.0, 1.0 - h / hH, h < hH);

    let texG = vec2f(textureDimensions(handLand, 0));
    let invT = vec2f(1.0) / texG;

    let xAfg = textureSample(handLand, handLandSampler, in.textuur + vec2f(invT.x, 0.0)).r
             - textureSample(handLand, handLandSampler, in.textuur - vec2f(invT.x, 0.0)).r;
    let yAfg = textureSample(handLand, handLandSampler, in.textuur + vec2f(0.0, invT.y)).r
             - textureSample(handLand, handLandSampler, in.textuur - vec2f(0.0, invT.y)).r;

    var tsjakka = normalize(vec3f(xAfg * 40.0, 1.0, yAfg * 40.0));
    let verlicht = l * (0.3 + 0.7 * dot(normalize(vec3f(-1.0, 1.0, 0.5)), tsjakka));

    const water = vec4f(0.0, 0.0, 0.5, 1.0);
    const rots  = vec4f(0.6, 0.5, 0.4, 1.0);
    const plant = vec4f(0.05, 0.6, 0.0, 1.0);

    uit.kleur = verlicht * mix(mix(plant, rots, r), water, w);

    return uit;
}