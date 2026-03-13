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
uniform float wave_scale : hint_range(0.1, 32.0) = 4.0;

uniform float emission_strength : hint_range(0.0, 10.0) = 1.0;
uniform float yellow_threshold : hint_range(0.0, 1.0) = 0.2;
uniform vec3 yellow_color : source_color = vec3(1.0, 0.9, 0.0);

varying flat float v_texture_index1;
varying flat int row_index;

float get_height_smooth(vec2 uv, float time) {
	float h = textureLod(height_map, uv, 0.0).r * height_scale;
	
	float t_mask = textureLod(control_map, uv, 0.0).r;
	float mask = (t_mask < 0.001) ? 1.0 : 0.0;
	float frequency = 6.283185 / wave_scale;
	float pos_x = uv.x * 256.0;
    float pos_z = uv.y * 256.0;
    float wave = sin(pos_x * frequency + TIME * wave_speed) 
               * cos(pos_z * frequency + TIME * wave_speed);
	wave = 0.0;
	return h + (mask * wave * wave_height);
}

void vertex() {
	vec2 global_uv = CUSTOM0.rg;
	vec2 main_uv = CUSTOM0.ba;
	
	float h = textureLod(height_map, global_uv, 0.0).r;
	VERTEX.y = h * height_scale;
	
	vec2 texel = 1.0 / vec2(textureSize(control_map, 0));	
	float t0 = textureLod(control_map, main_uv, 0.0).r;
    float t1 = textureLod(control_map, global_uv, 0.0).r;
    float t2 = textureLod(control_map, global_uv + vec2(0.0, -texel.y), 0.0).r;
    float t3 = textureLod(control_map, global_uv + vec2(-texel.x, 0.0), 0.0).r;
	float t4 = textureLod(control_map, global_uv + vec2(-texel.x, -texel.y), 0.0).r;
	v_texture_index1 = t0 * 255.0;
    float v_final = 0.0;
    if (t1 < 0.001 && t2 < 0.001 && t3 < 0.001 && t4 < 0.001) v_final = 1.0;
	
	float row_float = textureLod(control_map, main_uv, 0.0).g;
	row_index = int(round(row_float * 255.0));
	
	//vec3 world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	float pos_x = global_uv.x * 256.0;
    float pos_z = global_uv.y * 256.0;

	float frequency = 6.283185 / wave_scale;
    float wave = sin(pos_x * frequency + TIME * wave_speed) 
               * cos(pos_z * frequency + TIME * wave_speed);
    VERTEX.y += v_final * wave * wave_height;
	
// --- PŘESNÝ VÝPOČET NORMÁLY ---
	// Musíme definovat, jak velký kus světa (v metrech) představuje jeden texel UV
	// Změň '100.0' na skutečnou velikost tvého terénu (např. velikost Plane v Godotu)
	float terrain_size = 256.0; 
	vec2 tex_size = vec2(textureSize(height_map, 0));
	float step_n = terrain_size / tex_size.x; 

	// Vzorkujeme okolní výšky (včetně vln)
	float hL = get_height_smooth(global_uv + vec2(-texel.x, 0.0), TIME);
	float hR = get_height_smooth(global_uv + vec2(texel.x, 0.0), TIME);
	float hD = get_height_smooth(global_uv + vec2(0.0, -texel.y), TIME);
	float hU = get_height_smooth(global_uv + vec2(0.0, texel.y), TIME);

	// Vektory tečen (tangent a bitangent)
	// Rozdíl výšek dělíme vzdáleností mezi vzorky (2.0 * step)
	vec3 tangent = normalize(vec3(2.0 * step_n, hR - hL, 0.0));
	vec3 bitangent = normalize(vec3(0.0, hU - hD, 2.0 * step_n));
	
	// Normála je kolmice na tečny
	NORMAL = normalize(cross(bitangent, tangent));
	
	// Data pro fragment
	//v_texture_index1 = textureLod(control_map, main_uv, 0.0).r * 255.0;
	if (v_texture_index1 < 0.5)
		NORMAL = vec3(0.0, 1.0, 0.0);
	row_index = int(round(textureLod(control_map, main_uv, 0.0).g * 255.0));
}

void fragment() {
	//vec3 normal = normalize(cross(dFdy(VERTEX), dFdx(VERTEX)));//test nomals2
    //NORMAL = normal;//test nomals2
	
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
	
	vec2 atlas_res = vec2(textureSize(atlas_texture, 0));
    vec2 tile_uv_size = 1.0 / atlas_grid_size;
    vec2 border_uv = atlas_border_px / atlas_res;
	
	float atlas_col1 = mod(v_texture_index1, atlas_grid_size.x);
	float atlas_row1 = floor(v_texture_index1 / atlas_grid_size.x);
	vec2 atlas_offset1 = vec2(atlas_col1, atlas_row1) * tile_uv_size;
	vec2 min_uv1 = atlas_offset1 + border_uv ;
	vec2 max_uv1 = atlas_offset1 + tile_uv_size - border_uv ;
	vec2 final_uv1 = mix(min_uv1, max_uv1, rotated_uv);
	vec3 tex1 = texture(atlas_texture, final_uv1).rgb;
	float refl1 = texture(reflect_texture, final_uv1).r;
	
	ALBEDO = tex1;
	//ALBEDO = NORMAL * 0.5 + 0.5;//trest normals
	
	// Yellow detect
	float yellowness = 1.0 - length(tex1 - yellow_color);
	yellowness = smoothstep(yellow_threshold, 1.0, yellowness);	
	EMISSION = tex1 * yellowness * emission_strength;
	
	float target_roughness = 0.05;
	float target_metallic = 0.5;
	float default_roughness = 1.0;
	float default_metallic = 0.0;
	
	ROUGHNESS = mix(default_roughness, target_roughness, refl1);
	METALLIC = mix(default_metallic, target_metallic, refl1);
}