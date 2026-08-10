//WGSL vertex-shader voor geoDemo

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

struct VertexIn {
    @location(0) pos : vec3f,
    @location(1) tex : vec2f,
};

struct VertexUit {
    @builtin(position) positie : vec4f,
    @location(0) normaal : vec3f,
    @location(1) texU : vec2f,
    @location(2) kleur : vec4f,
};

@vertex
fn main(in : VertexIn) -> VertexUit {
    var uit : VertexUit;

    let mults = vec3f(33.0, 61.0, 12.0);

    uit.normaal = normalize(in.pos);
    uit.texU = in.tex;
    uit.kleur = vec4f(sin(in.tex.x * 3.142 * mults.x), sin(in.tex.y * 3.142 * mults.y), sin((in.tex.x + in.tex.y) * 3.142 * mults.z), 1.0);

    uit.positie = matrices.projectie * matrices.modelZicht * vec4f(in.pos, 1.0);

    return uit;
}