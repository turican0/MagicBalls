shader_type spatial;
render_mode unshaded, depth_test_disabled; 

uniform sampler2D label_texture;

void vertex() {
	MODELVIEW_MATRIX = VIEW_MATRIX * mat4(INV_VIEW_MATRIX[0], INV_VIEW_MATRIX[1], INV_VIEW_MATRIX[2], MODEL_MATRIX[3]);
	float dist = length(MODELVIEW_MATRIX[3].xyz);
	VERTEX *= dist * 0.1;
}

void fragment() {
	vec4 color = texture(label_texture, UV);
	ALBEDO = color.rgb;
	ALPHA = color.a;
}