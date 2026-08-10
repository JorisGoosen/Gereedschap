//WGSL fragment-shader voor toonHetLand

@group(1) @binding(0) var landTwee : texture_2d<f32>;

struct FragIn {
    @location(0) pixelPlek : vec2f,
};

struct FragUit {
    @location(0) FragColor : vec4f,
};

@fragment
fn main(in : FragIn) -> FragUit {
    var uit : FragUit;

    let textuurGrootte = textureDimensions(landTwee, 0);
    let texelPos = vec2i(vec2f(textuurGrootte) * in.pixelPlek);

    uit.FragColor = textureLoad(landTwee, texelPos, 0);

    return uit;
}