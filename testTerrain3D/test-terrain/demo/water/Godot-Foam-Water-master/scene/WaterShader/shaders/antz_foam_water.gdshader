shader_type spatial;
render_mode cull_disabled, world_vertex_coords, depth_draw_always, ambient_light_disabled, diffuse_burley;

//uniform float sync_time;
varying vec3 triplanar_pos;
uniform float wave_speed = 0.05; // the speed of the vertex waves
uniform float edge_fade_power = 2.0; // controls the contrast on the opacity fading when objects are on the surface
uniform float transmittence = 0.04; // how much light transmits through the water
uniform float h_dist_trans_weight = 3.0; // how much does the horizontal distance from the position affect its depth
uniform vec3 transmit_color : source_color; // a color correction applied to the screen texture coming from under the water

uniform float depth_fade_distance : hint_range(0.0, 100.0, 0.1) = 5.0; // the downward distance of the depth fade
uniform vec3 surface_albedo : source_color; // surface color on the top of the plane, in most areas this is overrriden by other colors, it will usually only be visible in shallow water
uniform vec4 surface_bottom : source_color; // the color of the underside of the water

uniform float opacity : hint_range(0.0, 5.0, 0.01) = 0.4; // general opacity amount
uniform float opacity_floor = 0.1; // opacity minimum
uniform float opacity_ceiling = 0.8; // opacity maximum

uniform bool pixelate = true;

uniform float roughness : hint_range(0.0, 1.0, 0.01) = 0.4;
uniform float height_scale = 1; // the scale of the waves in units
uniform float amplitude1 = 2; // the relative scale of vertex_noise_big
uniform float amplitude2 = 0.5; // the relative scale of vertex_noise_big2

uniform sampler2D vertex_noise_big : repeat_enable;
uniform sampler2D vertex_noise_big2 : repeat_enable;

uniform int v_noise_tile : hint_range(0, 300, 1) = 200; // tiling of the vertex_noise_texture in units

// 2 normal textures that move at different angles to eachother to create an effect
uniform sampler2D normal_noise : hint_normal, filter_linear_mipmap;
uniform sampler2D normal_noise2 : hint_normal, filter_linear_mipmap;

uniform float normal_noise_size = 51.2; // for correct scaling related to foam, this must be a multiple of 2 with thje decimal moved if you want it smaller
uniform float normal_noise_speed = 0.05;
uniform float v_normal_scale = 1; // scaling of the vertex normals effect
uniform int normal_map_w = 256; // this should be set to the size of your normal map texture, which is assumed to be square

uniform vec3 sky_color : source_color;
varying vec4 v_color;

group_uniforms v_color;
// to fake tramsittence, waves can have a different color as they get higher
uniform vec3 high_color : source_color;
uniform vec3 low_color : source_color;
varying vec3 wave_color;
uniform float wave_color_range = 2.0; // controls the power of the wave colors
uniform sampler2D screen_texture : hint_screen_texture;
uniform sampler2D depth_texture : hint_depth_texture;

group_uniforms foam;
uniform sampler2D foamTexture: repeat_enable;
uniform float foamScale: hint_range(1.0, 40, 1.0) = 25.0;
uniform float foamScrollSpeed: hint_range(0.1, 1.0, 0.1) = 0.4;
uniform float foamEdgeBias: hint_range(0.0, 1.0, 0.1) = 0.1;
uniform float foamFallOffDistance: hint_range(0.0, 1.0, 0.1) = 0.4;
uniform float foamEdgeDistance: hint_range(0.0, 1.0, 0.1) = 0.2;
const vec4 edgeFalloffColor = vec4(0.8,0.8,0.8,0.6);

float edge(float near, float far, float depth){
	depth = 1.0 - 2.0 * depth;
	return near * far / (far + depth * (near - far));
}

vec2 round_to_pixel(vec2 i, int width){
	float denom = 1.0 / float(width);
	float _x = i.x + abs(mod(i.x, denom) - denom);
	float _y = i.y + abs(mod(i.y , denom) - denom);
	return vec2(_x, _y);
}

float remap(float in_low, float in_high, float out_low, float out_high, float value){
	return out_low + (value - in_low) * (out_high - out_low) / (in_high - in_low);
}

// global uv
vec2 g_uv(vec2 uv, float speed, bool flipped, vec3 n) {
	vec2 _xy;
	_xy.x = uv.x;
	_xy.y = uv.y;

	float t_s = TIME * speed;

	if(!flipped) {
		_xy.x += t_s;
		_xy.y += t_s;
	} else {
		_xy.x -= t_s;
		_xy.y -= 0.0;
	}
	return _xy;
}

vec2 g_v(vec2 v, vec3 n, bool flipped){
	//float sync_time = TIME;
	float sync_time = 0.0;
	float f_v_n_t = float(v_noise_tile);

	v.x = mod(v.x, f_v_n_t);
	v.y = mod(v.y, f_v_n_t);

	vec2 _mapped = vec2(remap(0, f_v_n_t, 0, 1, v.x), remap(0, f_v_n_t, 0, 1, v.y));

	_mapped += n.xz;
	if(flipped) {
		_mapped.y -= (sync_time) * wave_speed;
	} else {
		_mapped.x += (sync_time) * wave_speed;
	}

	_mapped.x = mod(_mapped.x, 1);

	return _mapped;
}

float wave(vec2 y, vec3 n) {
	vec2 _y1 = g_v(y, n, false);
	vec2 _y2 = g_v(y + vec2(0.3, 0.476), n, true);

	float s = 0.0;
	s += texture(vertex_noise_big, mod(_y1, float(v_noise_tile))).r * amplitude1;
	s += texture(vertex_noise_big2, mod(_y2, float(v_noise_tile))).r * amplitude2;

	s -= height_scale/2.;
	return s;
}

varying mat4 camera_mix;

void vertex() {
	//npw = NODE_POSITION_WORLD;
	vec2 adj_v_pos = VERTEX.xz;

	float _height = wave(adj_v_pos, NODE_POSITION_WORLD) * height_scale;
	VERTEX.y += _height;

	float wave_color_mix = remap(-wave_color_range, wave_color_range, 0.0, 1.0, _height);

	wave_color_mix = clamp(wave_color_mix, 0.0, 1.0);
	wave_color = mix(low_color, high_color, wave_color_mix);

	vec2 e = vec2(0.1, 0.0);
	float v_scale = height_scale * v_normal_scale;

	vec3 normal = normalize(vec3(wave(adj_v_pos - e, NODE_POSITION_WORLD) * v_scale - wave(adj_v_pos + e, NODE_POSITION_WORLD) * v_scale, 1.0 * e.x, wave(adj_v_pos - e.yx, NODE_POSITION_WORLD) * v_scale - wave(adj_v_pos + e.yx, NODE_POSITION_WORLD) * v_scale));

	NORMAL = normal;
	triplanar_pos = VERTEX.xyz * vec3(1.0, 0, 1.0);
	v_color = COLOR.rgba;
	camera_mix = INV_VIEW_MATRIX;
}

void fragment() {
	ROUGHNESS = roughness;
	vec3 _albedo;
	vec3 deep;

	float depth3 = texture(depth_texture, SCREEN_UV).x;
	
	vec3 ndc3 = vec3(SCREEN_UV, depth3) * 2.0 - 1.0;
	vec4 world3 = camera_mix * INV_PROJECTION_MATRIX * vec4(ndc3, 1.0);
	vec3 world_position = world3.xyz / world3.w;

	float vertex_y = (INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).y;

	float _dist = distance(world_position.y, vertex_y);
	// the horizontal distance to the location, used to fade as the camera gets farther away
	float _h_dist = distance(triplanar_pos, world_position);

	_dist += _h_dist * h_dist_trans_weight;

	// Changes the color of geometry behind it as the water gets deeper
	float depth_fade_blend = exp(-_dist / depth_fade_distance);

	depth_fade_blend = clamp(depth_fade_blend, 0.0, 1.0);

	// Makes the water more transparent as it gets more shallow
	float alpha_blend = -_dist * transmittence;
	alpha_blend = clamp(1.0 - exp(alpha_blend), 0.7, 1.0);
	deep = mix(wave_color, surface_albedo, depth_fade_blend);

	// Antz - pixelate on/off
	vec2 rounded_uv;
	if (pixelate)
		rounded_uv = round_to_pixel((triplanar_pos.xz * (1.0 / normal_noise_size)) * 0.1, normal_map_w);
	else
		rounded_uv = (triplanar_pos.xz * (1.0 / normal_noise_size)) * 0.1;

	vec3 n_map1 = texture(normal_noise, g_uv(rounded_uv, normal_noise_speed, false, NODE_POSITION_WORLD)).xyz;
	vec3 n_map2 = texture(normal_noise2, g_uv(rounded_uv, normal_noise_speed, true, NODE_POSITION_WORLD)).xyz;

	NORMAL_MAP = mix(n_map1, n_map2, 0.5);

	if(FRONT_FACING) {
		vec3 under = texture(screen_texture, SCREEN_UV).rgb;
		_albedo = mix(under, deep, alpha_blend);
		float _ALPHA = remap(0.0, opacity, opacity_floor, opacity_ceiling, pow(alpha_blend, edge_fade_power));
		_albedo = mix(under + transmit_color, _albedo, _ALPHA);
	} else {
		NORMAL = -NORMAL;
		NORMAL_MAP = mix(vec3(1.0,1.0,1.0), vec3(0.0,0.0,0.0), NORMAL_MAP);
		vec3 over = texture(screen_texture, SCREEN_UV).rgb;
		_albedo = mix(over, surface_bottom.rgb, surface_bottom.a);
	}
	
	
	
	
	
	// Antz!!! FOAM!
	// set edge near/far to your camera near/far - TODO convert to uniform
	float z_depth = edge(0.05, 200, texture(depth_texture, SCREEN_UV).x); // depth texture
	float z_pos = edge(0.05, 200, FRAGCOORD.z); // from position
	float waterDepth = z_depth - z_pos; // diff
	waterDepth = max(waterDepth, 0.0); // make sure waterdepth >= 0

	float edgePatternScroll = TIME * foamScrollSpeed;
    
    vec2 scaledUV = rounded_uv * foamScale;
	
	// Calculate linear falloff value
    float falloff = 1.0 - (waterDepth / foamFallOffDistance) + foamEdgeBias;

    // Sample the mask
    float channelA = texture(foamTexture, scaledUV - vec2(edgePatternScroll, cos(rounded_uv.x))).r;
    float channelB = texture(foamTexture, scaledUV * 0.5 + vec2(sin(rounded_uv.y), edgePatternScroll)).b;

    // Modify it to our liking
    float mask = (channelA + channelB) * 0.95;
    mask = pow(mask, 2.0);
    mask = clamp(mask, 0.0, 1.0);
	
	 // Is this pixel in the leading edge?
    if(waterDepth < foamFallOffDistance * foamEdgeDistance) {
        // Modulate the surface alpha and the mask strength
        float leading = waterDepth / (foamFallOffDistance * foamEdgeDistance);
        //color.a *= leading;
		ALPHA *= leading; // Godot
        mask *= leading;
    }

    // Color the foam, blend based on alpha
    vec3 edge = edgeFalloffColor.rgb * falloff * edgeFalloffColor.a;




	
	ALBEDO = _albedo;
	ALBEDO = mix(sky_color, ALBEDO, v_color.a);
	
	// Subtract mask value from foam gradient, then add the foam value to the final pixel color
    ALBEDO += clamp(edge - vec3(mask), 0.0, 1.0);
}

group_uniforms lighting;

uniform float shine_strength : hint_range(0.0f, 1.0f) = 0.17f;
uniform float shine_shininess : hint_range(0.0f, 32.0f) = 18.0f;

uniform float shadow : hint_range(0.0, 1.0) = 0.72;
uniform float shadow_width : hint_range(0.001, 0.5) = 0.18;
uniform vec4 shadow_color: source_color = vec4(0.705);

uniform float _specular_smoothness : hint_range(0.0,0.5) = 0.199;
uniform float _specular_strength : hint_range(0.0,9.25) = 0.075;
uniform float _glossiness : hint_range(0.0,0.5) = 0.067;

// light shader code from: https://godotshaders.com/shader/toon/ and https://godotshaders.com/shader/flexible-toon-shader-godot-4/
void light() {
	vec3 H = normalize(VIEW + LIGHT);

	float NdotH = dot(NORMAL, H);
	float specular_amount = max(pow(NdotH, shine_shininess * shine_shininess), 0.0f) * ATTENUATION;
	SPECULAR_LIGHT += shine_strength * specular_amount * LIGHT_COLOR;

	float NdotL = dot(NORMAL, LIGHT) * ATTENUATION;
	NdotL = smoothstep(shadow - shadow_width, shadow + shadow_width, NdotL);

	// specular
	float specular_intensity = pow(NdotH, 1.0 / _glossiness);
	vec3 specular = vec3(smoothstep(0.5 - _specular_smoothness, 0.5 + _specular_smoothness, specular_intensity));

	DIFFUSE_LIGHT += ATTENUATION * mix(ALBEDO * shadow_color.rgb, (ALBEDO + (specular) * _specular_strength) * LIGHT_COLOR.rgb * 0.33, NdotL + 0.33/* * (smoothstep(1.0 -_rim_size - _rim_smoothness, 1.0 -_rim_size + _rim_smoothness, rimDot))*/);
	DIFFUSE_LIGHT = mix(sky_color, DIFFUSE_LIGHT, v_color.r);
	SPECULAR_LIGHT = mix(vec3(0.0), SPECULAR_LIGHT, v_color.r);
}
