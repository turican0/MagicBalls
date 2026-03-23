shader_type spatial;
render_mode unshaded, depth_test_disabled, blend_mix;

uniform sampler2D label_texture;
uniform float start_fading_dist = 10.0;
uniform float max_visible_dist = 20.0;

varying float dist_to_camera;

void vertex() {
	MODELVIEW_MATRIX = VIEW_MATRIX * mat4(INV_VIEW_MATRIX[0], INV_VIEW_MATRIX[1], INV_VIEW_MATRIX[2], MODEL_MATRIX[3]);
	dist_to_camera = length(MODELVIEW_MATRIX[3].xyz);
	VERTEX *= dist_to_camera * 0.1;
}

void fragment() {
	vec4 color = texture(label_texture, UV);
	float alpha_fade = 1.0 - smoothstep(start_fading_dist, max_visible_dist, dist_to_camera);
	ALBEDO = color.rgb;
	ALPHA = color.a * alpha_fade;
}