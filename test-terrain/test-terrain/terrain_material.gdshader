shader_type spatial;

uniform sampler2D height_map: filter_nearest, repeat_enable;
uniform sampler2D control_map : filter_nearest, repeat_enable;
uniform float height_scale : hint_range(0.0, 100.0) = 1.0;

uniform sampler2D atlas_texture : source_color;
uniform sampler2D reflect_texture : source_color;
uniform vec2 atlas_grid_size;
uniform vec2 atlas_border_px;

uniform float wave_height : hint_range(0.0, 1.0) = 0.5;
uniform float wave_speed : hint_range(0.1, 5.0) = 2.0;
uniform float wave_scale : hint_range(0.1, 10.0) = 4.0;

varying flat int v_texture_index1;

varying vec2 v_rotated_uv;

varying vec2 v_test_uv;

void vertex() {
    //v_texture_index1 = UV2.x;
	
	vec2 global_uv = CUSTOM0.rg;
	v_test_uv = global_uv;
	
	float h = textureLod(height_map, global_uv, 0.0).r;
	VERTEX.y = h * height_scale;	
	
	vec2 map_size = vec2(textureSize(control_map, 0));
	vec2 half_texel = 0.5 / map_size;
    vec2 shifted_uv = global_uv - half_texel; 
    ivec2 coords = ivec2(floor(shifted_uv * map_size + 0.5));
    coords = clamp(coords, ivec2(0), ivec2(map_size) - 1);    
    float t0 = texelFetch(control_map, coords, 0).r;
	float t1 = texelFetch(control_map, coords + ivec2(-1, 0), 0).r;
	float t2 = texelFetch(control_map, coords + ivec2(0, -1), 0).r;
	float t3 = texelFetch(control_map, coords + ivec2(-1, -1), 0).r;
	/*
	vec2 map_res = vec2(textureSize(control_map, 0));
    vec2 snapped_uv = (floor(global_uv * map_res) + 0.5) / map_res;
	
	vec2 texel = 1.0 / vec2(textureSize(control_map, 0));
    float t0 = textureLod(control_map, snapped_uv, 0.0).r;
    float t1 = textureLod(control_map, snapped_uv + vec2(-texel.x, 0.0), 0.0).r;
    float t2 = textureLod(control_map, snapped_uv + vec2(0.0, -texel.y), 0.0).r;
    float t3 = textureLod(control_map, snapped_uv + vec2(-texel.x, -texel.y), 0.0).r;
	*/
	v_texture_index1 = int(round(t0 * 255.0));
    float mask = 0.0;
    if (t0 < 0.001 && t1 < 0.001 && t2 < 0.001 && t3 < 0.001) {
        mask = 1.0;
    }
    float v_final = mask;
	
	//vec2 map_size = vec2(textureSize(control_map, 0));
	vec2 centered_global_uv = (floor(global_uv * map_size) + 0.5) / map_size;
	//float row_index = textureLod(control_map, centered_global_uv, 0.0).g;
	float row_index = texelFetch(control_map, coords, 0).g;
	int row_index2 = int(round(row_index * 255.0));
		//row_index2=3;
	if(row_index2 == 0 || row_index2 == 14 || row_index2 == 21 || row_index2 == 27) {
	    v_rotated_uv = vec2(UV.x, UV.y); // Bez rotace
	}
	else if(row_index2 == 1 || row_index2 == 15 || row_index2 == 22 || row_index2 == 24) {
	    v_rotated_uv = vec2(1.0 - UV.y, UV.x); // PŮVODNĚ 90° (nyní obráceno)
	}
	else if(row_index2 == 2 || row_index2 == 12 || row_index2 == 23 || row_index2 == 25) {
	    v_rotated_uv = vec2(1.0 - UV.x, 1.0 - UV.y); // 180°
	}
	else if(row_index2 == 3 || row_index2 == 13 || row_index2 == 20 || row_index2 == 26) {
	    v_rotated_uv = vec2(UV.y, 1.0 - UV.x); // PŮVODNĚ 270° (nyní obráceno)
	}
	else if(row_index2 == 4 || row_index2 == 10 || row_index2 == 19 || row_index2 == 29) {
	    v_rotated_uv = vec2(1.0 - UV.x, UV.y); // Flip X
	}
	else if(row_index2 == 5 || row_index2 == 11 || row_index2 == 16 || row_index2 == 30) {
	    v_rotated_uv = vec2(UV.y, UV.x); // Flip / Transpose
	}
	else if(row_index2 == 6 || row_index2 == 8 || row_index2 == 17 || row_index2 == 31) {
	    v_rotated_uv = vec2(UV.x, 1.0 - UV.y); // Flip Y
	}
	else if(row_index2 == 7 || row_index2 == 9 || row_index2 == 18 || row_index2 == 28) {
	    v_rotated_uv = vec2(1.0 - UV.y, 1.0 - UV.x); // Flip / Anti-transpose
	}
	else {
	    v_rotated_uv = UV; // Default
	}
	v_rotated_uv = UV;
	//v_rotated_uv = vec2(uv_table[base_idx]*0.5, uv_table[base_idx + 1]*0.5);
	//v_rotated_uv = vec2(1.0-UV.x, 1.0-UV.y);
	
	//float v_final = COLOR.g;
	
	vec3 world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;

    float wave = sin(world_pos.x / wave_scale + TIME * wave_speed) 
               * cos(world_pos.z / wave_scale + TIME * wave_speed);
    VERTEX.y += v_final * wave * wave_height;
}

void fragment() {
	//vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
	//vec2 tile_uv_size = 1.0 / atlas_grid_size;
	//vec2 half_pixel = vec2(0.5) / atlas_res;
	
	vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
    vec2 tile_uv_size = 1.0 / atlas_grid_size;
    vec2 border_uv = atlas_border_px / atlas_res;

	float atlas_col1 = float(v_texture_index1 % int(atlas_grid_size.x));
    float atlas_row1 = float(v_texture_index1 / int(atlas_grid_size.x));
	vec2 atlas_offset1 = vec2(atlas_col1, atlas_row1) * tile_uv_size;
	vec2 min_uv1 = atlas_offset1 + border_uv ;
	vec2 max_uv1 = atlas_offset1 + tile_uv_size - border_uv ;
	//vec2 final_uv1 = mix(min_uv1, max_uv1, UV);
	vec2 final_uv1 = mix(min_uv1, max_uv1, v_rotated_uv);
	vec3 tex1 = texture(atlas_texture, final_uv1).rgb;
	float refl1 = texture(reflect_texture, final_uv1).r;
	
	ALBEDO = tex1;
	
	float target_roughness = 0.05;
	float target_metallic = 0.5;
	float default_roughness = 1.0;
	float default_metallic = 0.0;
	
	ROUGHNESS = mix(default_roughness, target_roughness, refl1);
	METALLIC = mix(default_metallic, target_metallic, refl1);
}