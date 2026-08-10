//WGSL fragment-shader voor marcheerDemo: een heel kort ray-march over een hoogtekaart

struct Matrices {
    projectie : mat4x4f,
    modelZicht : mat4x4f,
    transInvMV : mat4x4f,
};

@group(0) @binding(1) var<uniform> matrices : Matrices;

@group(1) @binding(0) var landTwee : texture_2d<f32>;
@group(1) @binding(1) var landTweeSampler : sampler;

struct FragIn {
    @location(0) pixelPlek : vec3f,
};

struct FragUit {
    @location(0) FragColor : vec4f,
};

fn sdBox(p : vec3f, b : vec3f) -> f32 {
    let q = abs(p) - b;
    return length(max(q, vec3f(0.0))) + min(max(q.x, max(q.y, q.z)), 0.0);
}

@fragment
fn main(in : FragIn) -> FragUit {
    var uit : FragUit;

    const oog = vec3f(0.0, 1.0, -1.9);
    let straal = normalize(in.pixelPlek - (matrices.modelZicht * vec4f(oog, 0.0)).xyz);
    const stapje = 0.0015;

    var positie = in.pixelPlek;
    var hoogte = 0.0;

    var totaal = 0.0;
    const maxTotaal = 12.0;
    const water = vec4f(0.0, 0.0, 0.4, 1.0);
    const plant = vec4f(0.0, 0.5, 0.0, 1.0);
    const rots  = vec4f(0.5, 0.5, 0.5, 1.0);
    const strand = vec4f(0.5, 0.8, 0.2, 1.0);

    var afstand = 0.0;
    var vorigeAfstand = 0.0;
    const landDoos = vec3f(0.5, 0.5, 0.5);
    const plantHoogteMod = 0.02;
    var landKleur = rots;
    var kleurHier = rots;

    loop {
        if (totaal >= maxTotaal) { break; }

        vorigeAfstand = afstand;
        afstand = sdBox(positie + vec3f(0.0, 0.0, 0.5), landDoos);

        if (afstand > vorigeAfstand && afstand > 0.5) {
            uit.FragColor = water;
            return uit;
        }

        if (afstand < stapje) {
            let fPlek = positie.xz + vec2f(0.5, 1.0);
            kleurHier = textureSample(landTwee, landTweeSampler, fPlek);

            hoogte = kleurHier.r + kleurHier.g * plantHoogteMod;

            if (hoogte * landDoos.y > positie.y) {
                const waterHoogte = 0.025;

                if (hoogte < waterHoogte) {
                    landKleur = mix(water, plant, clamp(hoogte - kleurHier.g * plantHoogteMod, 0.0, waterHoogte) / waterHoogte);
                } else if (hoogte < 0.5) {
                    landKleur = mix(rots, plant, kleurHier.g);
                } else if (hoogte > 0.7) {
                    landKleur = mix(rots, vec4f(1.0), kleurHier.g);
                } else {
                    landKleur = rots;
                }

                landKleur *= kleurHier.b;

                uit.FragColor = landKleur;
                return uit;
            }

            totaal += stapje;
        } else {
            totaal += afstand;
        }

        positie = in.pixelPlek + (totaal * straal);
    }

    uit.FragColor = vec4f(totaal);

    return uit;
}