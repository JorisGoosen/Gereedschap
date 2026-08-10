//WGSL fragment-shader voor bewerkHetLand: maakt een "verlichte" hoogtekaart

@group(1) @binding(0) var landRuis : texture_2d<f32>;

struct FragIn {
    @location(0) pixelPlek : vec2f,
};

struct FragUit {
    @location(0) FragColor : vec4f,
};

var<private> hoogtes : array<f32, 9>;
var<private> textuurGrootte : vec2i;

fn haalHoogte(x : i32, y : i32) -> f32 {
    return hoogtes[x * 3 + y];
}

fn texelHoogte(x : i32, y : i32) -> f32 {
    let mx = clamp(x, 0, textuurGrootte.x);
    let my = clamp(y, 0, textuurGrootte.y);
    return textureLoad(landRuis, vec2i(mx, my), 0).r;
}

@fragment
fn main(in : FragIn) -> FragUit {
    var uit : FragUit;

    textuurGrootte = vec2i(textureDimensions(landRuis, 0));
    let texelPos = vec2i(vec2f(textuurGrootte) * in.pixelPlek);

    for (var x: i32 = -1; x < 2; x = x + 1) {
        for (var y: i32 = -1; y < 2; y = y + 1) {
            if (!(x == y && (x == -1 || x == 1))) {
                hoogtes[(x + 1) * 3 + (y + 1)] = texelHoogte(texelPos.x + x, texelPos.y + y);
            }
        }
    }

    let gradienten = abs(vec4f(
        haalHoogte(0, 1) - haalHoogte(1, 1),
        haalHoogte(2, 1) - haalHoogte(1, 1),
        haalHoogte(1, 0) - haalHoogte(1, 1),
        haalHoogte(1, 2) - haalHoogte(1, 1)));

    let maxGrad = max(max(gradienten.x, gradienten.y), max(gradienten.z, gradienten.w));
    let groente = clamp(1.0 - (maxGrad * 120.0), 0.0, 1.0);

    //de zon komt schuin van een kant
    const zonHoekPerStapje = 0.003;
    var huidigeZonHoogte = haalHoogte(1, 1);
    var volgendeZonHoogte : f32;
    var zonLicht = 1.0;
    var doorGaan = true;
    const zonStraal = vec2f(1.0, -0.5);
    var zonPos = vec2f(texelPos);

    loop {
        if (!doorGaan) { break; }
        if (zonPos.x < 0.0 || zonPos.y < 0.0 || zonPos.x >= f32(textuurGrootte.x) || zonPos.y >= f32(textuurGrootte.y)) { break; }

        huidigeZonHoogte += zonHoekPerStapje;
        volgendeZonHoogte = texelHoogte(i32(ceil(zonPos.x)), i32(ceil(zonPos.y)));

        if (volgendeZonHoogte >= huidigeZonHoogte) {
            zonLicht = 0.5;
            doorGaan = false;
        }

        if (huidigeZonHoogte > 1.0) {
            doorGaan = false;
        }

        zonPos += zonStraal;
    }

    uit.FragColor = vec4f(
        haalHoogte(1, 1),
        groente,
        zonLicht,
        mix(mix(gradienten.x, gradienten.y, 0.5), mix(gradienten.z, gradienten.w, 0.5), 0.5));

    return uit;
}