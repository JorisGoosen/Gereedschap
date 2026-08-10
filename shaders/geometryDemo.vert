//WGSL vertex-shader voor geometryDemo
//WebGPU kent geen geometry-shader; wat die deed (modelView/projectie toepassen + normaal)
//kan de vertex-shader zelf ook prima.

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
    @location(0) vPos : vec3f,
};

struct VertexUit {
    @builtin(position) positie : vec4f,
    @location(0) normaal : vec3f,
};

@vertex
fn main(in : VertexIn) -> VertexUit {
    var uit : VertexUit;

    uit.positie = matrices.projectie * matrices.modelZicht * vec4f(in.vPos, 1.0);
    uit.normaal = normalize(in.vPos);

    return uit;
}