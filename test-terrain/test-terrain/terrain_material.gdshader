shader_type spatial;

uniform sampler2D height_map;
uniform float height_scale : hint_range(0.0, 100.0) = 1.0;

uniform sampler2D atlas_texture : source_color;
uniform sampler2D reflect_texture : source_color;
uniform vec2 atlas_grid_size;
uniform vec2 atlas_border_px;

uniform float wave_height : hint_range(0.0, 1.0) = 0.5;
uniform float wave_speed : hint_range(0.1, 5.0) = 2.0;
uniform float wave_scale : hint_range(0.1, 10.0) = 4.0;

varying flat float v_texture_index1;

void vertex() {
    v_texture_index1 = UV2.x;
	
	vec2 global_uv = CUSTOM0.rg;	
	//float h = textureLod(height_map, global_uv, 0.0).r;
	//VERTEX.y = h * height_scale;
	
	float v_final = COLOR.g;
	
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

	float atlas_col1 = mod(v_texture_index1, atlas_grid_size.x);
	float atlas_row1 = floor(v_texture_index1 / atlas_grid_size.x);
	vec2 atlas_offset1 = vec2(atlas_col1, atlas_row1) * tile_uv_size;
	vec2 min_uv1 = atlas_offset1 + border_uv ;
	vec2 max_uv1 = atlas_offset1 + tile_uv_size - border_uv ;
	vec2 final_uv1 = mix(min_uv1, max_uv1, UV);
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