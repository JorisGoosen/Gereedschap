//WGSL fragment-shader voor quadDemo (voormalig quadDemo.frag)

struct FragIn {
    @location(0) tex : vec2f,
};

struct FragUit {
    @location(0) kleur : vec4f,
};

@fragment
fn main(in : FragIn) -> FragUit {
    var uit : FragUit;

    let golven = vec2f(1.0) + (2.0 * sin(in.tex * 33.0));
    let restkl  = 1.0 + (2.0 * cos(in.tex.y * in.tex.x * 133.7));

    uit.kleur = vec4f(golven, restkl, 1.0);

    return uit;
}