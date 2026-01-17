shader_type spatial;
// Přidáno blend_mix pro průhlednost a cull_disabled, aby byl vidět z obou stran
render_mode depth_draw_opaque, blend_mix, cull_disabled;

uniform float scale = 1.0;
uniform vec3 tex_speed = vec3( 0.0, -2.2, 0.5 );
uniform vec2 mokomoko = vec2( 0.1, 0.1 );

uniform float smoke_volume : hint_range( 0.0, 2.0 ) = 0.8;
uniform float smoke_aperture : hint_range( 0.0, 3.0 ) = 0.28;
uniform vec4 smoke_color : source_color = vec4( 0.185, 0.185, 0.185, 1.0 );

varying vec4 world_vertex;

float random( vec3 pos )
{
	return fract(sin(dot(pos, vec3(12.9898,78.233,-3.532532))) * 43758.5453);
}

float value_noise( vec3 pos )
{
	vec3 p = floor( pos );
	vec3 f = fract( pos );

	float v000 = random( p );
	float v100 = random( p + vec3( 1.0, 0.0, 0.0 ) );
	float v010 = random( p + vec3( 0.0, 1.0, 0.0 ) );
	float v110 = random( p + vec3( 1.0, 1.0, 0.0 ) );
	float v001 = random( p + vec3( 0.0, 0.0, 1.0 ) );
	float v101 = random( p + vec3( 1.0, 0.0, 1.0 ) );
	float v011 = random( p + vec3( 0.0, 1.0, 1.0 ) );
	float v111 = random( p + vec3( 1.0, 1.0, 1.0 ) );

	vec3 u = f * f * ( 3.0 - 2.0 * f );

	return mix(
		mix(
			mix( v000, v100, u.x )
		,	mix( v010, v110, u.x )
		,	u.y
		)
	,	mix(
			mix( v001, v101, u.x )
		,	mix( v011, v111, u.x )
		,	u.y
		)
	,	u.z
	);
}

float noise_tex( vec3 p )
{
	return (
		value_noise( p * 0.984864 ) * 0.5
	+	value_noise( p * 2.543 ) * 0.25
	+	value_noise( p * 9.543543 ) * 0.125
	+	value_noise( p * 21.65436 ) * 0.0625
	+	value_noise( p * 42.0 ) * 0.03125
	+	value_noise( p * 87.135148 ) * 0.015625
	+	value_noise( p * 340.66534654 ) * 0.0078125
	);
}

void vertex( )
{
	// Původní výpočet pro šum
	world_vertex = MODEL_MATRIX * vec4( VERTEX * scale, 1.0 );

	// Billboard logika:
	// Vytvoříme matici, která ignoruje rotaci uzlu a dívá se přímo na kameru
	mat4 model_view = VIEW_MATRIX * MODEL_MATRIX;
	model_view[0] = vec4(length(MODEL_MATRIX[0].xyz), 0.0, 0.0, 0.0);
	model_view[1] = vec4(0.0, length(MODEL_MATRIX[1].xyz), 0.0, 0.0);
	model_view[2] = vec4(0.0, 0.0, length(MODEL_MATRIX[2].xyz), 0.0);

	// Aplikujeme transformaci na vertex
	MODELVIEW_MATRIX = model_view;
}

void fragment( )
{
	float p[9];

	for( int y = 0; y < 3; y ++ ) {
		for( int x = 0; x < 3; x ++ ) {
			p[y*3 + x] = noise_tex( world_vertex.xyz + tex_speed * TIME + vec3( mokomoko * vec2( float(x - 1), float(y - 1) ), 0.0 ) );
		}
	}

	float smoke = clamp( sin( UV.x * 3.1415926535 ) * smoke_volume, 0.0, 1.0 );
	// Výpočet šumu kouře
	float smoke_noise = smoke * ( ( ( smoke + smoke_aperture ) * p[4] - smoke_aperture ) * 75.0 ) * UV.y;

	vec2 sobel_filter = clamp(
		vec2(
			( p[0] * -1.0 + p[3] * -2.0 + p[6] * -1.0 + p[2] * 1.0 + p[5] * 2.0 + p[8] * 1.0 )
		,	( p[0] * -1.0 + p[1] * -2.0 + p[2] * -1.0 + p[6] * 1.0 + p[7] * 2.0 + p[8] * 1.0 )
		)
	,	vec2( -1.0, -1.0 )
	,	vec2( 1.0, 1.0 )
	) * 0.5;

	NORMAL_MAP = normalize( vec3( sobel_filter.x + 0.5, -sobel_filter.y + 0.5, 1.0 ) );
	ALBEDO = smoke_color.rgb;

	// OPRAVA: Vynásobení vypočteného šumu průhledností z uniformy smoke_color
	ALPHA = clamp( smoke_noise * smoke_color.a, 0.0, 1.0 );
}