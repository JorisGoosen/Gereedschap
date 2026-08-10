//WGSL fragment-shader voor geometryDemo

struct FragIn {
    @location(0) normaal : vec3f,
};

struct FragUit {
    @location(0) kleur : vec4f,
};

@fragment
fn main(in : FragIn) -> FragUit {
    var uit : FragUit;

    uit.kleur = vec4f((vec3f(1.0) + in.normaal) / vec3f(2.0), 1.0);

    return uit;
}