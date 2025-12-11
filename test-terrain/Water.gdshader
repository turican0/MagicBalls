shader_type spatial;

uniform sampler2D Depth : hint_depth_texture, repeat_disable, filter_nearest;
uniform sampler3D Noise : repeat_enable;
uniform vec3 Color : source_color = vec3(1.0, 0.5, 0.0);

void vertex() {

	vec3 sample_pos = TIME * 0.1 + vec3(VERTEX.x, 0, VERTEX.z) * 0.05;

	float sample = texture(Noise, sample_pos).r;

	VERTEX.y += sample;

	float sample_px = texture(Noise, sample_pos + vec3( 0.05, 0, 0)).r;
	float sample_nx = texture(Noise, sample_pos + vec3(-0.05, 0, 0)).r;
	float sample_pz = texture(Noise, sample_pos + vec3(0, 0,  0.05)).r;
	float sample_nz = texture(Noise, sample_pos + vec3(0, 0, -0.05)).r;

	NORMAL = vec3((sample_nx - sample_px) / 2.0, 1.0, (sample_nz - sample_pz) / 2.0);
}

void fragment() {

	ALBEDO = Color;

	METALLIC = 0.0;
	SPECULAR = 1.0;
	ROUGHNESS = 0.0;

	// use depth texture to make shallow areas more translucent
	float depth = texture(Depth, SCREEN_UV).x;
	//vec3 ndc = vec3(SCREEN_UV, depth) * 2.0 - 1.0;
	vec3 ndc = vec3(SCREEN_UV * 2.0 - 1.0, depth);
	vec4 view = INV_PROJECTION_MATRIX * vec4(ndc, 1.0);
	view.xyz /= view.w;
	float linear_depth = -view.z;

	float object_depth = FRAGCOORD.z;
	//vec3 object_ndc = vec3(SCREEN_UV, object_depth) * 2.0 - 1.0;
	vec3 object_ndc = vec3(SCREEN_UV * 2.0 - 1.0, object_depth);
	vec4 object_view = INV_PROJECTION_MATRIX * vec4(object_ndc, 1.0);
	object_view.xyz /= object_view.w;
	float linear_object_depth = -object_view.z;

	ALPHA = (smoothstep(0.0, 4.0, linear_depth - linear_object_depth) + 1.0) / 2.0;
}
