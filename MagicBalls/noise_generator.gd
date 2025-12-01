# CustomNoise2DGenerator.gd
extends VoxelGeneratorScript

# --- Parametry (Kopie C++ vlastností) ---
# POZNÁMKA: V GDScriptu nelze vytvářet vlastní @export proměnné na VoxelGeneratorScript.
# Místo toho přednačítáme zdroje (Resources).
const NOISE_RESOURCE = preload("res://noise.tres") 
const CURVE_RESOURCE = preload("res://curve.tres") # Může být null, pokud křivka není definována.

const ISO_SCALE: float = 1.0
const TERRAIN_AMPLITUDE: float = 100.0 # Jak vysoko se terén může tyčit
const BASE_HEIGHT: float = 0.0         # Základní Y-pozice

func _get_used_channels_mask() -> int:
	# Používáme SDF, i když jde o Heightmap, protože Transvoxel mesher potřebuje SDF
	return VoxelBuffer.CHANNEL_SDF

func _generate_block(out_buffer: VoxelBuffer, origin: Vector3i, lod: int) -> void:
	if NOISE_RESOURCE == null:
		# V C++ kódu je kontrola ERR_FAIL_COND_V(params.noise.is_null(), result);
		return

	var size: Vector3i = out_buffer.get_size()
	var stride: int = 1 << lod 
	
	# --- Hlavní Generační Logika ---
	
	var gz: int = origin.z
	for z in range(size.z):
		var gx: int = origin.x
		for x in range(size.x):
			
			# 1. Získání 2D šumu (Noise)
			# Dělíme, abychom šum "roztáhli" a získali větší tvary terénu
			var noise_scale_factor: float = 0.01 
			var noise_value: float = NOISE_RESOURCE.get_noise_2d(
				float(gx) * noise_scale_factor, 
				float(gz) * noise_scale_factor
			)
			
			# C++ Vzorec: 0.5 + 0.5 * noise_2d (Normalizace na rozsah 0.0 až 1.0)
			var normalized_noise: float = 0.5 + 0.5 * noise_value
			
			# 2. Aplikace Křivky (Curve)
			var curve_sampled_value: float
			
			if CURVE_RESOURCE != null:
				# C++ Vzorec: curve.sample_baked(normalized_noise)
				# Křivka nám umožní definovat, jak se má výška měnit (např. zploštit vrcholky)
				# Poznámka: V Godot editoru musíte křivku "Upéct" (Bake), aby byla data k dispozici pro skript.
				curve_sampled_value = CURVE_RESOURCE.sample(normalized_noise) # V GDScriptu použijeme .sample()
			else:
				curve_sampled_value = normalized_noise
			
			# 3. Výpočet finální Světové Výšky Povrchu
			# Vypočítáme absolutní Y-pozici terénu.
			var terrain_y_surface: float = BASE_HEIGHT + curve_sampled_value * TERRAIN_AMPLITUDE
			
			# 4. Procházení Y-osy a Výpočet SDF
			var gy: int = origin.y
			for y in range(size.y):
				
				# SDF = Aktuální Y pozice - Výška povrchu
				# Vytvoření SDF distance polem (Heightmap je jen speciální případ SDF)
				var sdf_value: float = float(gy) - terrain_y_surface
				
				# Zápis s měřítkem
				var final_scaled_sdf: float = ISO_SCALE * sdf_value
				
				out_buffer.set_voxel_f(final_scaled_sdf, x, y, z, VoxelBuffer.CHANNEL_SDF)
				
				gy += stride
			
			gx += stride
		gz += stride
		
	# C++ volá out_buffer.compress_uniform_channels(); 
	# V GDScriptu se o to postará modul, ale můžete to zkusit přidat, pokud by to pomohlo:
	# out_buffer.compress_uniform_channels()
