shader_type spatial;

// --- UNIFORMY (ponecháno podle tvého zadání) ---
uniform sampler2D atlas_texture : source_color;
uniform sampler2D reflect_texture : source_color;
uniform vec2 atlas_grid_size;
uniform float text_weight : hint_range(0.0, 1.0) = 0.5;

varying flat float v_texture_index1;
varying flat float v_texture_index2;

void vertex() {
    v_texture_index1 = floor(UV2.x + 0.1);
	v_texture_index2 = floor(UV2.x + 0.1);
	//text_weight = COLOR.r;
}

void fragment() {
	vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
	vec2 tile_uv_size = 1.0 / atlas_grid_size;
	vec2 half_pixel = vec2(0.5) / atlas_res;

	float atlas_col1 = mod(v_texture_index1, atlas_grid_size.x);
	float atlas_row1 = floor(v_texture_index1 / atlas_grid_size.x);
	vec2 atlas_offset1 = vec2(atlas_col1, atlas_row1) * tile_uv_size;
	vec2 min_uv1 = atlas_offset1 + half_pixel;
	vec2 max_uv1 = atlas_offset1 + tile_uv_size - half_pixel;
	vec2 final_uv1 = clamp(atlas_offset1 + (UV * tile_uv_size), min_uv1, max_uv1);
	vec3 tex1 = texture(atlas_texture, final_uv1).rgb;
	float refl1 = texture(atlas_texture, final_uv1).r/256.0;
	
	float atlas_col2 = mod(v_texture_index2, atlas_grid_size.x);
	float atlas_row2 = floor(v_texture_index2 / atlas_grid_size.x);
	vec2 atlas_offset2 = vec2(atlas_col2, atlas_row2) * tile_uv_size;
	vec2 min_uv2 = atlas_offset2 + half_pixel;
	vec2 max_uv2 = atlas_offset2 + tile_uv_size - half_pixel;
	vec2 final_uv2 = clamp(atlas_offset2 + (UV * tile_uv_size), min_uv2, max_uv2);
	vec3 tex2 = texture(atlas_texture, final_uv2).rgb;
	float refl2 = texture(atlas_texture, final_uv1).r/256.0;
	
	vec3 final_color = mix(tex1, tex2, text_weight);
	ALBEDO = final_color;
	
	float final_refl = mix(refl1, refl2, text_weight);
	float target_roughness = 0.05;
	float target_metallic = 0.5;
	float default_roughness = 1.0;
	float default_metallic = 0.0;
	
	//if(refl1!=0.0)
	//final_refl=0.5;
	
	ROUGHNESS = mix(default_roughness, target_roughness, final_refl);
	METALLIC = mix(default_metallic, target_metallic, final_refl);
}