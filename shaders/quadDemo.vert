//WGSL vertex-shader voor quadDemo (voormalig quadDemo.vert)

struct beeldParameters {
    schermBreedte : f32,
    schermHoogte : f32,
    schermVerhouding : f32,
    _opvulling : f32,
};

//De schermparameters uit de basis-bind-groep (kan gerust ongebruikt zijn)
@group(0) @binding(0) var<uniform> beeld : beeldParameters;

struct VertexIn {
    @location(0) vPos : vec2f,
    @location(1) vTex : vec2f,
};

struct VertexUit {
    @builtin(position) positie : vec4f,
    @location(0) tex : vec2f,
};

@vertex
fn main(in : VertexIn) -> VertexUit {
    var uit : VertexUit;
    uit.positie = vec4f(in.vPos, 0.0, 1.0);
    uit.tex = in.vTex;
    return uit;
}