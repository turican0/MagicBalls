shader_type spatial;

// --- UNIFORMY (ponecháno podle tvého zadání) ---
uniform sampler2D atlas_texture : source_color;
uniform sampler2D reflect_texture : source_color;
uniform vec2 atlas_grid_size;
uniform vec2 atlas_border_px;
varying float v_weight;

varying flat float v_texture_index1;
varying flat float v_texture_index2;

void vertex() {
    v_texture_index1 = UV2.x;
	v_texture_index2 = UV2.y;
	v_weight = COLOR.r;
}

void fragment() {
	//vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
	//vec2 tile_uv_size = 1.0 / atlas_grid_size;
	//vec2 half_pixel = vec2(0.5) / atlas_res;
	
	vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
    vec2 tile_uv_size = 1.0 / atlas_grid_size;
    vec2 border_uv = atlas_border_px / atlas_res;

	float atlas_col1 = mod(v_texture_index1, atlas_grid_size.x);
	float atlas_row1 = floor(v_texture_index1 / atlas_grid_size.x);
	vec2 atlas_offset1 = vec2(atlas_col1, atlas_row1) * tile_uv_size;
	vec2 min_uv1 = atlas_offset1 + border_uv ;
	vec2 max_uv1 = atlas_offset1 + tile_uv_size - border_uv ;
	vec2 final_uv1 = mix(min_uv1, max_uv1, UV);
	vec3 tex1 = texture(atlas_texture, final_uv1).rgb;
	float refl1 = texture(reflect_texture, final_uv1).r;
	
	float atlas_col2 = mod(v_texture_index2, atlas_grid_size.x);
	float atlas_row2 = floor(v_texture_index2 / atlas_grid_size.x);
	vec2 atlas_offset2 = vec2(atlas_col2, atlas_row2) * tile_uv_size;
	vec2 min_uv2 = atlas_offset2 + border_uv ;
	vec2 max_uv2 = atlas_offset2 + tile_uv_size - border_uv ;
	vec2 final_uv2 = mix(min_uv2, max_uv2, UV);
	vec3 tex2 = texture(atlas_texture, final_uv2).rgb;
	float refl2 = texture(reflect_texture, final_uv2).r;
	
	vec3 final_color = mix(tex1, tex2, v_weight);
	ALBEDO = final_color;
	
	float final_refl = mix(refl1, refl2, v_weight);
	float target_roughness = 0.05;
	float target_metallic = 0.5;
	float default_roughness = 1.0;
	float default_metallic = 0.0;
	
	ROUGHNESS = mix(default_roughness, target_roughness, final_refl);
	METALLIC = mix(default_metallic, target_metallic, final_refl);
}