//WGSL vertex-shader voor landDemo

struct BeeldParameters {
    schermBreedte : f32,
    schermHoogte : f32,
    schermVerhouding : f32,
    _opvulling : f32,
};

@group(0) @binding(0) var<uniform> beeld : BeeldParameters;

struct Matrices {
    projectie : mat4x4f,
    modelZicht : mat4x4f,
    transInvMV : mat4x4f,
};

@group(0) @binding(1) var<uniform> matrices : Matrices;

struct ExtraParameters {
    vermindering : f32,
    reserve1 : f32,
    reserve2 : f32,
    reserve3 : f32,
};

@group(0) @binding(2) var<uniform> extra : ExtraParameters;

@group(1) @binding(0) var handLand : texture_2d<f32>;
@group(1) @binding(1) var handLandSampler : sampler;

struct VertexIn {
    @location(0) vPos : vec3f,
    @location(1) vTex : vec2f,
};

struct VertexUit {
    @builtin(position) pos : vec4f,
    @location(0) textuur : vec2f,
    @location(1) hoogte : f32,
};

@vertex
fn main(in : VertexIn) -> VertexUit {
    var uit : VertexUit;

    uit.textuur = in.vTex;

    var pos = vec4f(in.vPos, 1.0);
    pos.y = textureSampleLevel(handLand, handLandSampler, uit.textuur, extra.vermindering).r * 0.5;
    uit.hoogte = pos.y;

    uit.pos = matrices.projectie * matrices.modelZicht * pos;

    return uit;
}