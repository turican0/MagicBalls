shader_type canvas_item;
uniform sampler2D SCREEN_TEXTURE : hint_screen_texture, filter_linear_mipmap;
uniform vec3 MyGain = vec3(1.0);
uniform vec3 MyOffset = vec3(0.0);
void fragment() {
    vec3 screen_color = texture(SCREEN_TEXTURE, SCREEN_UV).rgb;

    vec3 final_color = (screen_color + MyOffset) * MyGain;

    final_color = clamp(final_color, 0.0, 1.0);

    COLOR.rgb = final_color;
}