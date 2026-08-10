//WGSL vertex-shader voor computeDemo: instanced vierkantjes voor de schapen en wolven

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
    @location(0) vierkant : vec2f,
    @location(1) plaatsKleur : vec4f,
};

struct VertexUit {
    @builtin(position) positie : vec4f,
    @location(0) kleur : vec4f,
};

@vertex
fn main(in : VertexIn) -> VertexUit {
    var uit : VertexUit;

    uit.positie = matrices.projectie * matrices.modelZicht * vec4f(in.vierkant + in.plaatsKleur.xy, 0.0, 1.0);
    uit.kleur = vec4f(in.plaatsKleur.w, 0.0, in.plaatsKleur.z, 1.0);

    return uit;
}