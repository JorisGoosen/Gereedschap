//WGSL fragment-shader voor geoDemo

struct FragIn {
    @location(0) normaal : vec3f,
    @location(1) texU : vec2f,
    @location(2) kleur : vec4f,
};

struct FragUit {
    @location(0) uitKleur : vec4f,
};

@fragment
fn main(in : FragIn) -> FragUit {
    var uit : FragUit;

    uit.uitKleur = in.kleur;

    return uit;
}