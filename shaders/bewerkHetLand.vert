//WGSL vertex-shader voor bewerkHetLand (deelt het scherm-vierkant met toonHetLand)

struct BeeldParameters {
    schermBreedte : f32,
    schermHoogte : f32,
    schermVerhouding : f32,
    _opvulling : f32,
};

@group(0) @binding(0) var<uniform> beeld : BeeldParameters;

struct VertexIn {
    @location(0) vPos : vec2f,
    @location(1) vTex : vec2f,
};

struct VertexUit {
    @builtin(position) positie : vec4f,
    @location(0) pixelPlek : vec2f,
};

@vertex
fn main(in : VertexIn) -> VertexUit {
    var uit : VertexUit;

    uit.positie = vec4f(in.vPos, 0.0, 1.0);
    uit.pixelPlek = vec2f(0.5) + (0.5 * in.vTex);

    return uit;
}