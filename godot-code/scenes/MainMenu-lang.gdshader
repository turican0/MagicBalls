shader_type canvas_item;

uniform float amplitude = 0.02;
uniform float frequency = 10.0;
uniform float speed = 2.0;
uniform float alpha : hint_range(0.0, 1.0) = 0.2; // průhlednost

void fragment() {
    vec2 uv = UV;
    uv.y += sin(uv.x * frequency + TIME * speed) * amplitude;
    vec4 tex = texture(TEXTURE, uv);
    tex.a *= alpha;   // násobení původní alfy
    COLOR = tex;
}
