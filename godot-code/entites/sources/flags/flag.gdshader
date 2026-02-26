shader_type spatial;
// Přidán blend_mix pro podporu průhlednosti
render_mode cull_disabled, blend_mix; 

uniform float speed = 2.0;
uniform float frequency_y = 5.0;
uniform float frequency_x = 2.5;
uniform float frequency_z = 2.5;
uniform float amplitude_y = 0.1;
uniform float amplitude_x = 0.05;
uniform float amplitude_z = 0.05;
uniform float inclination = -0.04;

uniform sampler2D tex: source_color;

void fragment(){
	vec2 safe_uv = clamp(UV, vec2(0.002), vec2(0.998));
	vec4 albedo_tex = texture(tex, safe_uv);
	ALBEDO = albedo_tex.rgb;
	
	// TENTO ŘÁDEK AKTIVUJE PRŮHLEDNOST
	ALPHA = albedo_tex.a;
	ALPHA_SCISSOR_THRESHOLD = 0.5;
	
	// Pokud chceš mít ostré hrany bez "duchů", odkomentuj řádek níže:
	// ALPHA_SCISSOR_THRESHOLD = 0.5;

	if (!FRONT_FACING) {
		NORMAL = -NORMAL;
	}
}

void vertex(){
	VERTEX.y += sin((UV.x - TIME * speed) * frequency_y) * amplitude_y * UV.x;
	VERTEX.x += cos((UV.y - TIME * speed) * frequency_x) * amplitude_x * 0.1 * UV.x;
	VERTEX.z += sin((UV.x - TIME * speed) * frequency_z) * amplitude_z * UV.x;
	VERTEX.x -= (1.0 - UV.y) * inclination;
}