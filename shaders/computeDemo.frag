//WGSL fragment-shader voor computeDemo

struct FragIn {
    @location(0) kleur : vec4f,
};

struct FragUit {
    @location(0) kleur : vec4f,
};

@fragment
fn main(in : FragIn) -> FragUit {
    var uit : FragUit;

    uit.kleur = in.kleur;

    return uit;
}