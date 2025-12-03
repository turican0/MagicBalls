extends Terrain3D

# Funkce pro aplikaci poškození – volitelně exportovaná pro snadné nastavení v editoru
@export var default_radius: float = 10.0
@export var default_strength: float = -5.0

func _apply_terrain_damage(point: Vector3, radius: float = default_radius, strength: float = default_strength) -> void:
	reset_terrain_height_for_testing()
	var current_data: Terrain3DData = get_data()
	if not current_data:
		push_error("Terrain3DData not available!")
		return
	
	# Vertex spacing z tohoto nodu
	var local_vertex_spacing: float = self.vertex_spacing  # Přímo z self
	
	# Iterace přes mřížku bodů v kruhu
	var steps: int = int(radius / local_vertex_spacing) * 2 + 1
	var center_x: float = point.x
	var center_z: float = point.z
	
	var half_steps = int(steps / 2.0) # Vynutí float dělení (steps / 2.0) a poté usekne desetinnou část
	for x_offset in range(-half_steps, half_steps + 1):
		for z_offset in range(-half_steps, half_steps + 1):
			var pos: Vector3 = Vector3(
				center_x + x_offset * vertex_spacing,
				0.0,
				center_z + z_offset * vertex_spacing
			)
			
			var distance: float = Vector2(pos.x, pos.z).distance_to(Vector2(point.x, point.z))
			if distance > radius:
				continue
			
			var current_height: float = data.get_height(pos)
			if is_nan(current_height):
				continue
			
			# Tvá funkce úbytku (lineární falloff – uprav podle potřeby)
			var falloff: float = 1.0 - (distance / radius)
			var damage: float = strength * falloff
			
			var new_height: float = current_height + damage
			data.set_height(pos, new_height)
	
	# Manuální update regionu pro jistotu (Terrain3D to obvykle zvládne automaticky)
	#notify_region_modified(point - Vector3(radius, 0, radius), point + Vector3(radius, 0, radius))
	
func reset_terrain_height_for_testing() -> void:
	# Konstanta pro výškovou mapu (index 0) - Toto je správné z API Terrain3DRegion/Data
	const MAP_HEIGHT: int = 0

	# 1. Získání Dictionary všech regionů
	# Klíč: Vector2i (pozice regionu), Hodnota: Terrain3DRegion
	var all_regions: Dictionary = data.get_regions_all()

	if all_regions.is_empty():
		push_warning("Terrain3D has no regions initialized. Resetting nothing.")
		return

	var regions_reset_count: int = 0

	# 2. Iterace přes všechny objekty Terrain3DRegion a volání interní fill_map
	# POZNÁMKA: Metoda fill_map(map_type, fill_value) existuje přímo na Třídě Terrain3DRegion!
	for region_location in all_regions.keys():
		var region: Terrain3DRegion = all_regions[region_location]

		# Získání velikosti a formátu výškové mapy z existujícího regionu
		var current_height_map: Image = region.get_map(MAP_HEIGHT)
		if current_height_map.is_empty():
			push_warning("Region at " + str(region_location) + " has no height map data.")
			continue

		#if first_region_size.x == 0:
			#first_region_size = current_height_map.get_size()

		# Vytvoření nové Image, která má stejnou velikost a formát (FORMAT_RF pro 32-bit float)
		# a která bude inicializována nulami (0.0)
		var blank_image := Image.create(
		current_height_map.get_width(), 
		current_height_map.get_height(), 
		false, 
		Image.FORMAT_RF # 32-bit float
		)
		# Poznámka: Image.create() vytvoří Image, kde jsou všechny pixely defaultně nulové.

		# Nahrazení původní mapy nulovou mapou
		region.set_map(MAP_HEIGHT, blank_image)

		# Přepočítání výškového rozsahu (min/max), který bude nyní (0, 0)
		region.calc_height_range() 

		# Označení regionu jako modifikovaného, aby byl uložen a aktualizován
		region.set_modified(true) 
		regions_reset_count += 1

	# 3. Aktualizace a notifikace Godotu o masivní změně

	# A. Vynucení aktualizace všech map (rebuild indexů a textur)
	data.update_maps(Terrain3DRegion.MapType.TYPE_MAX, true, false) 

	# B. Získání celkových hranic pro notifikaci Terrain3D uzlu
	#var terrain_bounds: AABB = data.get_bounds()
	#var half_size_x: float = terrain_bounds.size.x / 2.0
	#var half_size_z: float = terrain_bounds.size.z / 2.0

	# Upozornění na změnu celého rozsahu terénu pro aktualizaci kolize
	#notify_region_modified(
	#Vector3(terrain_bounds.position.x - half_size_x, 0, terrain_bounds.position.z - half_size_z), 
	#Vector3(terrain_bounds.position.x + half_size_x, 0, terrain_bounds.position.z + half_size_z)
	#)

	print("Vynulování výšky terénu dokončeno. Resetováno regionů: " + str(regions_reset_count))
