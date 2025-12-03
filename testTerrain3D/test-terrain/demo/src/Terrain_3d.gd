extends Terrain3D

# Funkce pro aplikaci poškození – volitelně exportovaná pro snadné nastavení v editoru
@export var default_radius: float = 10.0
@export var default_strength: float = -5.0

func _apply_terrain_damage(point: Vector3, radius: float = default_radius, strength: float = default_strength) -> void:
	var data: Terrain3DData = get_data()
	if not data:
		push_error("Terrain3DData not available!")
		return
	
	# Vertex spacing z tohoto nodu
	var vertex_spacing: float = self.vertex_spacing  # Přímo z self
	
	# Iterace přes mřížku bodů v kruhu
	var steps: int = int(radius / vertex_spacing) * 2 + 1
	var center_x: float = point.x
	var center_z: float = point.z
	
	for x_offset in range(-steps / 2, steps / 2 + 1):
		for z_offset in range(-steps / 2, steps / 2 + 1):
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
