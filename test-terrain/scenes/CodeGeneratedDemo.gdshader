shader_type spatial;
render_mode unshaded, depth_test_disabled, cull_disabled, fog_disabled;

uniform vec3 MyGain;
uniform vec3 MyOffset;
uniform sampler2D screen_texture : hint_screen_texture, filter_linear, repeat_disable;

void vertex() {
    // VERTEX.xy jsou souřadnice Quadu (při velikosti 2.0 jsou to -1.0 a 1.0)
    // Nastavením POSITION přímo na tyto hodnoty ignorujeme pozici ve světě
    POSITION = vec4(VERTEX.xy, 1.0, 1.0);
}

void fragment() {
    // SCREEN_UV zajistí, že čteme pixely přesně tam, kde se nacházíme
    vec3 scene = texture(screen_texture, SCREEN_UV).rgb;

    // Aplikace VGA filtru
    vec3 corrected = (scene * MyGain) + MyOffset;

    ALBEDO = corrected;
}