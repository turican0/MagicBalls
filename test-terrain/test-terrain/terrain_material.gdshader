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

const float uv_table[256] = float[](
	0.,0., 1.,0., 1.,1., 0.,1.,  1.,0., 1.,1., 0.,1., 0.,0.,  1.,1., 0.,1., 0.,0., 1.,0.,  0.,1., 0.,0., 1.,0., 1.,1.,
	1.,0., 0.,0., 0.,1., 1.,1.,  0.,0., 0.,1., 1.,1., 1.,0.,  0.,1., 1.,1., 1.,0., 0.,0.,  1.,1., 1.,0., 0.,0., 0.,1.,
	0.,1., 1.,1., 1.,0., 0.,0.,  1.,1., 1.,0., 0.,0., 0.,1.,  1.,0., 0.,0., 0.,1., 1.,1.,  0.,0., 0.,1., 1.,1., 1.,0.,
	1.,1., 0.,1., 0.,0., 1.,0.,  0.,1., 0.,0., 1.,0., 1.,1.,  0.,0., 1.,0., 1.,1., 0.,1.,  1.,0., 1.,1., 0.,1., 0.,0.,
	0.,0., 0.,1., 1.,1., 1.,0.,  0.,1., 1.,1., 1.,0., 0.,0.,  1.,1., 1.,0., 0.,0., 0.,1.,  1.,0., 0.,0., 0.,1., 1.,1.,
	0.,1., 0.,0., 1.,0., 1.,1.,  0.,0., 1.,0., 1.,1., 0.,1.,  1.,0., 1.,1., 0.,1., 0.,0.,  1.,1., 0.,1., 0.,0., 1.,0.,
	1.,0., 1.,1., 0.,1., 0.,0.,  1.,1., 0.,1., 0.,0., 1.,0.,  0.,1., 0.,0., 1.,0., 1.,1.,  0.,0., 1.,0., 1.,1., 0.,1.,
	1.,1., 0.,1., 0.,0., 1.,0.,  1.,0., 0.,0., 0.,1., 1.,1.,  0.,0., 0.,1., 1.,1., 1.,0.,  0.,1., 1.,1., 1.,0., 0.,0.
);

varying vec2 v_rotated_uv;

void vertex() {
    //v_texture_index1 = UV2.x;
	
	vec2 global_uv = CUSTOM0.rg;
	
	float h = textureLod(height_map, global_uv, 0.0).r;
	VERTEX.y = h * height_scale;
	
	vec2 texel = 1.0 / vec2(textureSize(control_map, 0));
    float t0 = textureLod(control_map, global_uv, 0.0).r;
    float t1 = textureLod(control_map, global_uv + vec2(-texel.x, 0.0), 0.0).r;
    float t2 = textureLod(control_map, global_uv + vec2(0.0, -texel.y), 0.0).r;
    float t3 = textureLod(control_map, global_uv + vec2(-texel.x, -texel.y), 0.0).r;
	v_texture_index1 = t0 * 255.0;
    float mask = 0.0;
    if (t0 < 0.001 && t1 < 0.001 && t2 < 0.001 && t3 < 0.001) {
        mask = 1.0;
    }
    float v_final = mask;
	
	float row_index = textureLod(control_map, global_uv, 0.0).b;
	int row_index2 = int(floor(row_index * 255.0 + 0.5)); // Kanál B pro rotaci
	row_index2 = clamp(row_index2, 0, 31); // Bezpečnostní pojistka
	int vertex_id = 0;
	int type_index = int(CUSTOM0.b * 100.0 + 0.5);
	//if (type_index == 1)
	{
		if (UV.x < 0.5 && UV.y < 0.5) vertex_id = 1;
		else if (UV.x > 0.5 && UV.y < 0.5) vertex_id = 2;
		else vertex_id = 3;
	}
	//else if (type_index == 2)
	//{
		//if (UV.x < 0.5 && UV.y < 0.5) vertex_id = 1;
		//else if (UV.x > 0.5 && UV.y > 0.5) vertex_id = 3;
		//else vertex_id = 4;
	//}
	//else if (type_index == 3)
	//{
		//if (UV.x > 0.5 && UV.y < 0.5) vertex_id = 2;
		//else if (UV.x > 0.5 && UV.y > 0.5) vertex_id = 3;
		//else vertex_id = 4;
	//}
	//else
	//{
		//if (UV.x > 0.5 && UV.y < 0.5) vertex_id = 2;
		//else if (UV.x < 0.5 && UV.y > 0.5) vertex_id = 4;
		//else vertex_id = 1;
	//}

	int base_idx = row_index2 * 8 + 0 * 2;
	v_rotated_uv = vec2(uv_table[base_idx]*0.5, uv_table[base_idx + 1]*0.5);
	//v_rotated_uv = vec2(uv_table[0], uv_table[0 + 1]);
	
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

	float atlas_col1 = mod(v_texture_index1, atlas_grid_size.x);
	float atlas_row1 = floor(v_texture_index1 / atlas_grid_size.x);
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