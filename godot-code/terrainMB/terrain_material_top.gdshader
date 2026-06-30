shader_type spatial;
render_mode cull_front;

uniform sampler2D height_map: filter_nearest, repeat_enable;
uniform sampler2D control_map : filter_nearest, repeat_enable;
uniform float height_scale : hint_range(0.0, 100.0) = 1.0;

uniform sampler2D atlas_texture : source_color, filter_linear_mipmap_anisotropic;
uniform sampler2D atlas_texture_nearest : source_color, filter_nearest_mipmap;
uniform sampler2D reflect_texture : source_color, filter_linear_mipmap;
uniform vec2 atlas_grid_size;
uniform vec2 atlas_border_px;
uniform bool nearest_filtering = true;

uniform float wave_height : hint_range(0.0, 1.0) = 0.5;
uniform float wave_speed : hint_range(0.1, 5.0) = 2.0;
uniform float wave_scale : hint_range(0.1, 32.0) = 4.0;

varying flat int row_index;

void vertex() {
	vec2 global_uv = CUSTOM0.rg;
	float h = textureLod(height_map, global_uv, 0.0).r;
	VERTEX.y = h * height_scale;
	float terrain_size = 256.0; 
	vec2 tex_size = vec2(textureSize(height_map, 0));
	vec2 texel = 1.0 / tex_size;
	float step_n = terrain_size / tex_size.x;
	float hL = textureLod(height_map, global_uv + vec2(-texel.x, 0.0), 0.0).r * height_scale;
	float hR = textureLod(height_map, global_uv + vec2(texel.x, 0.0), 0.0).r * height_scale;
	float hD = textureLod(height_map, global_uv + vec2(0.0, -texel.y), 0.0).r * height_scale;
	float hU = textureLod(height_map, global_uv + vec2(0.0, texel.y), 0.0).r * height_scale;
	vec3 tangent = normalize(vec3(2.0 * step_n, hR - hL, 0.0));
	vec3 bitangent = normalize(vec3(0.0, hU - hD, 2.0 * step_n));
	NORMAL = normalize(cross(bitangent, tangent));
}

void fragment() {
	if (!FRONT_FACING) {
		NORMAL = -NORMAL;
	}
	float texture_index = 1.0;
	vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
	vec2 tile_uv_size = 1.0 / atlas_grid_size;
	vec2 border_uv = atlas_border_px / atlas_res;
	float atlas_col = mod(texture_index, atlas_grid_size.x);
	float atlas_row = floor(texture_index / atlas_grid_size.x);
	vec2 atlas_offset = vec2(atlas_col, atlas_row) * tile_uv_size;
	vec2 min_uv = atlas_offset + border_uv;
	vec2 max_uv = atlas_offset + tile_uv_size - border_uv;
	vec2 final_uv = mix(min_uv, max_uv, UV);
	if (nearest_filtering) {
		ALBEDO = texture(atlas_texture_nearest, final_uv).rgb;
	} else {
		ALBEDO = texture(atlas_texture, final_uv).rgb;
	}
	ROUGHNESS = 1.0;
	METALLIC = 0.0;
}