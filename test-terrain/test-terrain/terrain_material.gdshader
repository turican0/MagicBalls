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

varying flat float v_texture_index1;
varying flat int row_index;

//varying vec2 v_rotated_uv;

//varying vec2 v_global_uv;

void vertex() {
    //v_texture_index1 = UV2.x;
	
	vec2 global_uv = CUSTOM0.rg;
	vec2 main_uv = CUSTOM0.ba;
	
	float h = textureLod(height_map, global_uv, 0.0).r;
	VERTEX.y = h * height_scale;
	
	vec2 texel = 1.0 / vec2(textureSize(control_map, 0));
	//bool is_flipped = UV.x < 0.5;
	//vec2 target_offset = vec2(0,0);
	//if (!is_flipped)
		//target_offset = vec2(-texel.x, 0.0);
	//float truex = textureLod(control_map, target_offset + global_uv, 0.0).b;
	//float truey = textureLod(control_map, target_offset + global_uv, 0.0).a;
	//target_offset = vec2(0,-texel.y);
	//if (!is_flipped)
		//target_offset = vec2(-texel.x, -texel.y);
	//vec2 target_uv = textureLod(control_map, target_offset + global_uv, 0.0).ba;
	//vec2 size = vec2(textureSize(control_map, 0));
	//vec2 snapped_target_uv = (floor(target_uv * size) + 0.5) / size;
	//float t0x = textureLod(control_map, snapped_target_uv, 0.0).r;
	//float t0x = textureLod(control_map, main_uv, 0.0).r;
	//v_texture_index1 = t0x * 255.0;
	
	float t0 = textureLod(control_map, main_uv, 0.0).r;
    float t1 = textureLod(control_map, global_uv, 0.0).r;
    float t2 = textureLod(control_map, global_uv + vec2(0.0, -texel.y), 0.0).r;
    float t3 = textureLod(control_map, global_uv + vec2(-texel.x, -texel.y), 0.0).r;
	float t4 = textureLod(control_map, global_uv + vec2(-texel.x, -texel.y), 0.0).r;
	v_texture_index1 = t0 * 255.0;
    float mask = 0.0;
    if (t1 < 0.001 && t2 < 0.001 && t3 < 0.001 && t4 < 0.001) mask = 1.0;
    float v_final = mask;
	
	// PŘESUNUTÁ LOGIKA Z VERTEXU
	//vec2 texel = 1.0 / vec2(textureSize(control_map, 0));
	//bool is_flipped = UV.x < 0.5;
	//vec2 target_offset = vec2(0,0);
	//if (!is_flipped) target_offset = vec2(-texel.x, 0.0);
	
	//vec2 sample_uv = v_global_uv + target_offset;
	//vec4 control_sample = textureLod(control_map, sample_uv, 0.0);
	float row_float = textureLod(control_map, main_uv, 0.0).g;
	row_index = int(round(row_float * 255.0));
	//v_rotated_uv = vec2(uv_table[base_idx]*0.5, uv_table[base_idx + 1]*0.5);
	//v_rotated_uv = vec2(1.0-UV.x, 1.0-UV.y);
	
	//float v_final = COLOR.g;
	
	vec3 world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;

    float wave = sin(world_pos.x / wave_scale + TIME * wave_speed) 
               * cos(world_pos.z / wave_scale + TIME * wave_speed);
    VERTEX.y += v_final * wave * wave_height;
}

void fragment() {
	
	// Výpočet rotace UV přímo ve fragmentu (používáme nativní UV)
	vec2 rotated_uv;
	if(row_index == 0 || row_index == 14 || row_index == 21 || row_index == 27) {
		rotated_uv = UV;
	}
	else if(row_index == 1 || row_index == 15 || row_index == 22 || row_index == 24) {
		rotated_uv = vec2(1.0 - UV.y, UV.x);
	}
	else if(row_index == 2 || row_index == 12 || row_index == 23 || row_index == 25) {
		rotated_uv = vec2(1.0 - UV.x, 1.0 - UV.y);
	}
	else if(row_index == 3 || row_index == 13 || row_index == 20 || row_index == 26) {
		rotated_uv = vec2(UV.y, 1.0 - UV.x);
	}
	else if(row_index == 4 || row_index == 10 || row_index == 19 || row_index == 29) {
		rotated_uv = vec2(1.0 - UV.x, UV.y);
	}
	else if(row_index == 5 || row_index == 11 || row_index == 16 || row_index == 30) {
		rotated_uv = vec2(UV.y, UV.x);
	}
	else if(row_index == 6 || row_index == 8 || row_index == 17 || row_index == 31) {
		rotated_uv = vec2(UV.x, 1.0 - UV.y);
	}
	else if(row_index == 7 || row_index == 9 || row_index == 18 || row_index == 28) {
		rotated_uv = vec2(1.0 - UV.y, 1.0 - UV.x);
	}
	else {
		rotated_uv = UV;
	}
	//rotated_uv = UV;
	//vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
	//vec2 tile_uv_size = 1.0 / atlas_grid_size;
	//vec2 half_pixel = vec2(0.5) / atlas_res;
	
	vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
    vec2 tile_uv_size = 1.0 / atlas_grid_size;
    vec2 border_uv = atlas_border_px / atlas_res;
	
	float atlas_col1 = mod(v_texture_index1, atlas_grid_size.x);
	float atlas_row1 = floor(v_texture_index1 / atlas_grid_size.x);
	//if(row_index<1)
	//{
		//atlas_col1 = mod(0.0, atlas_grid_size.x);
		//atlas_row1 = floor(0.0 / atlas_grid_size.x);
	//}
	vec2 atlas_offset1 = vec2(atlas_col1, atlas_row1) * tile_uv_size;
	vec2 min_uv1 = atlas_offset1 + border_uv ;
	vec2 max_uv1 = atlas_offset1 + tile_uv_size - border_uv ;
	//vec2 final_uv1 = mix(min_uv1, max_uv1, UV);
	vec2 final_uv1 = mix(min_uv1, max_uv1, rotated_uv);
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