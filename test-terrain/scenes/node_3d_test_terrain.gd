extends Node3D

## --- PARAMETRY TERÉNU ---
const GRID_SIZE = 256        # Počet buněk (čtverců) na jedné ose (64x64)
const VERTEX_COUNT = GRID_SIZE + 1 # Počet vrcholů na jedné ose (65x65)
const CELL_SCALE = 1.0      # Velikost jedné buňky v herních jednotkách

## --- DATové STRUKTURY ---
# Pole pro uložení pozic vrcholů (Vector3, X/Y jsou pozice, Z je výška)
var vertices: Array[Array] = [] 
# Pole pro uložení indexu textury pro každý čtverec
var texture_indices: Array[Array] = []

## --- UZLY ---
var mesh_instance: MeshInstance3D
var surface_tool: SurfaceTool

func _ready():
	# 1. Inicializace a nastavení uzlů
	initialize_nodes()
	
	# 2. Inicializace dat sítě (vrcholy a textury)
	initialize_grid_data()
	
	# 3. Generování a vykreslení sítě
	recalculate_mesh()

## --- FÁZE 1: Inicializace ---

func initialize_nodes():
	mesh_instance = MeshInstance3D.new()
	mesh_instance.name = "TerrainMesh"
	add_child(mesh_instance)
	
	# Zde načtěte váš ShaderMaterial (který odkazuje na Texture Atlas)
	var material = load("res://test-terrain/terrain_material.tres")
	if material:
		mesh_instance.material_override = material
	else:
		# Použijte alespoň standardní materiál pro vizuální kontrolu, pokud se nepodaří načíst
		printerr("Chyba: Nepodařilo se načíst terrain_material.tres.")
		mesh_instance.material_override = StandardMaterial3D.new()

func initialize_grid_data():
	# Inicializace pole pro vrcholy (X/Y pozice a náhodná výška Z)
	vertices.resize(VERTEX_COUNT)
	for x in range(VERTEX_COUNT):
		vertices[x] = []
		vertices[x].resize(VERTEX_COUNT)
		for y in range(VERTEX_COUNT):
			# Nastavení počáteční pozice a náhodné výšky pro demonstraci
			var height = randf_range(-1.0, 1.0) * 0.5 
			vertices[x][y] = Vector3(x * CELL_SCALE, height, y * CELL_SCALE)
			
	# Inicializace pole pro indexy textur pro každý čtverec
	texture_indices.resize(GRID_SIZE)
	for x in range(GRID_SIZE):
		texture_indices[x] = []
		texture_indices[x].resize(GRID_SIZE)
		for y in range(GRID_SIZE):
			# Nastavení náhodného indexu textury (např. 0, 1, 2)
			texture_indices[x][y] = randi_range(0, 24) 

## --- FÁZE 2: Generování sítě ---

func recalculate_mesh():
	# SurfaceTool je nástroj pro snadné programové vytváření sítě
	surface_tool = SurfaceTool.new()
	surface_tool.begin(Mesh.PRIMITIVE_TRIANGLES)

	# OPRAVA CHYBY: Používá SurfaceTool.WINDING_CLOCKWISE místo Mesh.WINDING_CLOCKWISE
	#surface_tool.set_winding_from_front_face(SurfaceTool.WINDING_CLOCKWISE)

	for x in range(GRID_SIZE):
		for y in range(GRID_SIZE):

			# 1. Získání 4 vrcholů pro aktuální čtverec
			var v1 = vertices[x][y]
			var v2 = vertices[x+1][y]
			var v3 = vertices[x+1][y+1]
			var v4 = vertices[x][y+1]

			# 2. Vypočítání indexu textury pro tento čtverec
			var texture_index = texture_indices[x][y]

			# 3. Rozhodnutí o triangulaci
			var diff_v1_v3 = abs(v1.y - v3.y)
			var diff_v2_v4 = abs(v2.y - v4.y)

			if diff_v1_v3 < diff_v2_v4:
				# Možnost 1: Úhlopříčka V1 -> V3
				add_triangle(v1, v2, v3, texture_index)
				add_triangle(v1, v3, v4, texture_index)
			else:
				# Možnost 2: Úhlopříčka V2 -> V4
				add_triangle(v2, v3, v4, texture_index)
				add_triangle(v2, v4, v1, texture_index)

	# Dokončení sítě
	surface_tool.generate_normals()
	surface_tool.index() 

	mesh_instance.mesh = surface_tool.commit()

## --- POMOCNÁ FUNKCE ---

# Přidá trojúhelník do SurfaceTool s nastavením UV mapování
func add_triangle(p1: Vector3, p2: Vector3, p3: Vector3, texture_index: int):
	# UV mapování je zde zjednodušeno pro každý trojúhelník na rohy (0,0), (1,0), (0,1).
	# Ve Fragment Shaderu se toto UV použije pro mapování na oblast v Texture Atlasu.
	
	# UV mapování v rámci jednoho čtverce:
	var uv_p1 = Vector2(0.0, 0.0) 
	var uv_p2 = Vector2(1.0, 0.0)
	var uv_p3 = Vector2(0.0, 1.0)
	
	# 1. Vrchol P1
	# UV2.x posílá index textury do shaderu.
	surface_tool.set_uv2(Vector2(texture_index, 0)) 
	surface_tool.set_uv(uv_p1)
	surface_tool.add_vertex(p1)
	
	# 2. Vrchol P2
	surface_tool.set_uv2(Vector2(texture_index, 0))
	surface_tool.set_uv(uv_p2)
	surface_tool.add_vertex(p2)
	
	# 3. Vrchol P3
	surface_tool.set_uv2(Vector2(texture_index, 0))
	surface_tool.set_uv(uv_p3)
	surface_tool.add_vertex(p3)

## --- PŘÍKLAD MODIFIKACE (volitelné) ---

# Funkce, která by se volala, kdykoli se změní výška:
func modify_vertex_height(x: int, y: int, new_height: float):
	if x >= 0 and x < VERTEX_COUNT and y >= 0 and y < VERTEX_COUNT:
		# Změna výšky vrcholu
		vertices[x][y].y = new_height
		
		# Po změně vždy přepočítat celou síť (nebo jen dotčené čtverce)
		# Pro zjednodušení voláme recalculate_mesh() celou:
		recalculate_mesh()
