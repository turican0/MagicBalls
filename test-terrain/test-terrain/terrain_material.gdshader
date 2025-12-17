shader_type spatial;

// --- UNIFORMY (ponecháno podle tvého zadání) ---
uniform sampler2D atlas_texture : source_color;
uniform vec2 atlas_grid_size; 

varying flat float v_texture_index;

void vertex() {
    // Předání indexu z UV2
    // Přidáváme 0.1 a floor(), aby index 49.0 nebyl kvůli chybě floatu brán jako 48.99
    v_texture_index = floor(UV2.x + 0.1);
}

void fragment() {
	// 1. Zjištění rozměrů atlasu v pixelech
	vec2 atlas_res = vec2(textureSize(atlas_texture, 0));

	// 2. Výpočet velikosti jedné dlaždice v UV souřadnicích (0.0 až 1.0)
	vec2 tile_uv_size = 1.0 / atlas_grid_size;

	// 3. Převod 1D indexu na 2D pozici buňky (sloupec, řádek)
	float atlas_col = mod(v_texture_index, atlas_grid_size.x);
	float atlas_row = floor(v_texture_index / atlas_grid_size.x);

	// 4. Počáteční offset dlaždice v atlasu
	vec2 atlas_offset = vec2(atlas_col, atlas_row) * tile_uv_size;

	// --- OPRAVA BLIKÁNÍ: HALF-PIXEL INSET ---
	// Vypočítáme velikost půl pixelu v UV prostoru
	vec2 half_pixel = vec2(0.5) / atlas_res;

	// Definujeme bezpečnou zónu uvnitř dlaždice (posuneme okraje o půl pixelu dovnitř)
	vec2 min_uv = atlas_offset + half_pixel;
	vec2 max_uv = atlas_offset + tile_uv_size - half_pixel;

	// Standardní výpočet UV pozice
	vec2 raw_uv = atlas_offset + (UV * tile_uv_size);

	// Ořízneme UV souřadnice do bezpečné zóny (tím zmizí blikání na indexu 49 a dalších)
	vec2 final_uv = clamp(raw_uv, min_uv, max_uv);

	// 5. Vzorkování
	ALBEDO = texture(atlas_texture, final_uv).rgb;
}