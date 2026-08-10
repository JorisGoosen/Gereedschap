//WGSL vertex-shader voor marcheerDemo

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
    @location(0) vPos : vec2f,
    @location(1) vTex : vec2f,
};

struct VertexUit {
    @builtin(position) positie : vec4f,
    @location(0) pixelPlek : vec3f,
};

@vertex
fn main(in : VertexIn) -> VertexUit {
    var uit : VertexUit;

    uit.positie = vec4f(in.vPos, 0.0, 1.0);

    //de bovenste 3x3 van modelZicht: M * (v, 0).xyz
    uit.pixelPlek = (matrices.modelZicht * vec4f(in.vTex.x, (in.vTex.y / beeld.schermVerhouding) + 0.5, -1.0, 0.0)).xyz;

    return uit;
}