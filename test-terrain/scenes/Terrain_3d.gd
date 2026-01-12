# Funkce pro aplikaci poškození – volitelně exportovaná pro snadné nastavení v editoru
@export var default_radius: float = 10.0
@export var default_strength: float = -5.0

	# Manuální update regionu pro jistotu (Terrain3D to obvykle zvládne automaticky)
	#notify_region_modified(point - Vector3(radius, 0, radius), point + Vector3(radius, 0, radius))
	
