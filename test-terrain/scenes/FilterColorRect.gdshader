shader_type canvas_item;

uniform sampler2D SCREEN_TEXTURE : hint_screen_texture, filter_linear_mipmap;

uniform vec3 MyGain = vec3(1.0);
uniform vec3 MyOffset = vec3(0.0);
uniform float MySatMultiplier = 1.0;  // 1.0 = neutrální, >1 boostuje saturaci

vec3 rgb_to_hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void fragment() {
    vec3 screen_color = texture(SCREEN_TEXTURE, SCREEN_UV).rgb;
    vec3 color = (screen_color + MyOffset) * MyGain;
    color = clamp(color, 0.0, 1.0);
    if (MySatMultiplier != 1.0) {
        vec3 hsv = rgb_to_hsv(color);
        hsv.y *= MySatMultiplier;
        hsv.y = clamp(hsv.y, 0.0, 1.0);
        color = hsv_to_rgb(hsv);
    }

    COLOR.rgb = color;
}