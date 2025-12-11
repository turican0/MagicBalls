shader_type spatial;

// Uniform pro Texture Atlas (jedna velka textura s vice texturami terenu)
uniform sampler2D atlas_texture;

// Uniform pro informaci o rozmisteni textur v atlasu
// Napr. Vector2(4.0, 4.0) pokud je atlas 4x4
uniform vec2 atlas_grid_size; 

// Vlastní data, která posíláte z GDScriptu přes SurfaceTool.set_uv2() nebo set_custom().
// Napr. V1.uv2.x a V2.uv2.x a V3.uv2.x muzou obsahovat ID textury (napr. 0, 1, 2)
varying float texture_index;

void vertex() {
    // STANDARDNI KOD PRO TRANSFORMACI A UV MAPOVANI
    
    // Predani indexu textury do Fragment Shaderu
    // Predpokladame, ze index textury je ulozen v UV2 (uv2.x)
    texture_index = UV2.x; 
    
    // UV mapovani pro Terrain Atlas
    // Zde je nutne spocitat finalni UV souradnice pro Atlas
    // K tomu vsak potrebujeme vedet, ktere ze 4 moznych bodu rohu 
    // ctverce se prave zpracovava (pro mapovani 0,0 - 1,1)
    
    // Zatim pouzijeme standardni UV mapovani a vyber textury provedeme ve fragmentu:
    // UV = UV; 
}

void fragment() {
    // 1. Spoctete pozici textury v atlasu
    // Napr. pro index 5 v 4x4 atlasu (indexy 0-15) to je radek 1, sloupec 1
    
    float size_x = 1.0 / atlas_grid_size.x;
    float size_y = 1.0 / atlas_grid_size.y;
    
    // Prevod 1D indexu na 2D souradnice bunky (celá čísla)
    float atlas_col = mod(texture_index, atlas_grid_size.x); // Sloupec (0, 1, 2, ...)
    float atlas_row = floor(texture_index / atlas_grid_size.x); // Radek (0, 1, 2, ...)
    
    // Pocatecni bod (offset) textury v atlasu
    vec2 atlas_offset = vec2(atlas_col * size_x, atlas_row * size_y);
    
    // 2. Finalni UV souradnice pro Texture Atlas
    // UV - je standardni UV souradnice (0.0 - 1.0) v ramci jednoho trojuhelniku
    // Finalni UV je: (standardni UV * velikost_bunky) + offset
    vec2 final_uv = (UV * vec2(size_x, size_y)) + atlas_offset;
    
    // 3. Vzorkovani textury a nastaveni albeda
    ALBEDO = texture(atlas_texture, final_uv).rgb;
}