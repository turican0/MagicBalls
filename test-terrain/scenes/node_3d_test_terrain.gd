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

var uv_table_d4350: Array = [
	[0, 0, 1, 0, 1, 1, 0, 1],
	[1, 0, 1, 1, 0, 1, 0, 0],
	[1, 1, 0, 1, 0, 0, 1, 0],
	[0, 1, 0, 0, 1, 0, 1, 1],
	[1, 0, 0, 0, 0, 1, 1, 1],
	[0, 0, 0, 1, 1, 1, 1, 0],
	[0, 1, 1, 1, 1, 0, 0, 0],
	[1, 1, 1, 0, 0, 0, 0, 1],

	[0, 1, 1, 1, 1, 0, 0, 0],
	[1, 1, 1, 0, 0, 0, 0, 1],
	[1, 0, 0, 0, 0, 1, 1, 1],
	[0, 0, 0, 1, 1, 1, 1, 0],
	[1, 1, 0, 1, 0, 0, 1, 0],
	[0, 1, 0, 0, 1, 0, 1, 1],
	[0, 0, 1, 0, 1, 1, 0, 1],
	[1, 0, 1, 1, 0, 1, 0, 0],

	[0, 0, 0, 1, 1, 1, 1, 0],
	[0, 1, 1, 1, 1, 0, 0, 0],
	[1, 1, 1, 0, 0, 0, 0, 1],
	[1, 0, 0, 0, 0, 1, 1, 1],
	[0, 1, 0, 0, 1, 0, 1, 1],
	[0, 0, 1, 0, 1, 1, 0, 1],
	[1, 0, 1, 1, 0, 1, 0, 0],
	[1, 1, 0, 1, 0, 0, 1, 0],

	[1, 0, 1, 1, 0, 1, 0, 0],
	[1, 1, 0, 1, 0, 0, 1, 0],
	[0, 1, 0, 0, 1, 0, 1, 1],
	[0, 0, 1, 0, 1, 1, 0, 1],
	[1, 1, 1, 0, 0, 0, 0, 1],
	[1, 0, 0, 0, 0, 1, 1, 1],
	[0, 0, 0, 1, 1, 1, 1, 0],
	[0, 1, 1, 1, 1, 0, 0, 0]
]

func _ready():
	# 1. Inicializace a nastavení uzlů
	initialize_nodes()
	
	# 2. Inicializace dat sítě (vrcholy a textury)
	initialize_grid_data()
	
	# 3. Generování a vykreslení sítě
	get_parent().get_node("DecodeLevel").init()
	recalculate_mesh()	
	for y in range(GRID_SIZE):
		for x in range(GRID_SIZE):
			texture_indices[x][y]=get_parent().get_node("DecodeLevel").mapTerrainType_10B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]
	for y in range(VERTEX_COUNT):
		for x in range(VERTEX_COUNT):
			vertices[x][y].y=get_parent().get_node("DecodeLevel").mapHeightmap_11B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]*0.1
	recalculate_mesh()
	
	#begin of Multimesh
	var mmi:MultiMeshInstance3D = get_parent().get_node("MultiMeshInstance3D")
	# 1. Pokud už MultiMesh v uzlu existuje, použijeme ho, jinak vytvoříme nový
	if not mmi.multimesh:
		mmi.multimesh = MultiMesh.new()
		mmi.multimesh.transform_format = MultiMesh.TRANSFORM_3D
	# 2. Přiřadíme tvůj vygenerovaný mesh
	mmi.multimesh.mesh = mesh_instance.mesh
	
	# Překopíruje materiál z hlavního terénu na kopie
	mmi.material_override = mesh_instance.material_override 
	# Pokud nepoužíváš override, zkus:
	if not mmi.material_override:
		mmi.material_override = mesh_instance.mesh.surface_get_material(0)
	# 3. Nastavíme pozice kopií (mřížka 3x3 bez středu)
	var offset = GRID_SIZE * CELL_SCALE
	var positions = []
	for x in [-1, 0, 1]:
		for z in [-1, 0, 1]:
			if x == 0 and z == 0: continue # Vynechá místo původního terénu
			positions.append(Vector3(x * offset, 0, z * offset))
	# 4. Aplikujeme počet a transformace
	mmi.multimesh.instance_count = positions.size()
	for i in range(positions.size()):
		var t = Transform3D(Basis(), positions[i])
		mmi.multimesh.set_instance_transform(i, t)
	#end of Multimesh

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
			var textUV_42:int = ((get_parent().get_node("DecodeLevel").mapAngle_13B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)] >> 2) & 0x1C)
			var rPoint1 = Vector2(uv_table_d4350[textUV_42][0],uv_table_d4350[textUV_42][1])
			var rPoint2 = Vector2(uv_table_d4350[textUV_42][2],uv_table_d4350[textUV_42][3])
			var rPoint3 = Vector2(uv_table_d4350[textUV_42][4],uv_table_d4350[textUV_42][5])
			var rPoint4 = Vector2(uv_table_d4350[textUV_42][6],uv_table_d4350[textUV_42][7])

			# 1. Získání 4 vrcholů pro aktuální čtverec
			var v1 = vertices[x][y]
			var v2 = vertices[x+1][y]
			var v3 = vertices[x+1][y+1]
			var v4 = vertices[x][y+1]
			
			#var v1 = vertices[x+rPoint1.x][y+rPoint1.y]
			#var v2 = vertices[x+rPoint2.x][y+rPoint2.y]
			#var v3 = vertices[x+rPoint3.x][y+rPoint3.y]
			#var v4 = vertices[x+rPoint4.x][y+rPoint4.y]

			# 2. Vypočítání indexu textury pro tento čtverec
			var texture_index = texture_indices[x][y]

			# 3. Rozhodnutí o triangulaci
			var diff_v1_v3 = abs(v1.y - v3.y)
			var diff_v2_v4 = abs(v2.y - v4.y)

			if ((x+y+1)&1):
				# Možnost 1: Úhlopříčka V1 -> V3
				add_triangle(v1, v2, v3, texture_index,rPoint1,rPoint2,rPoint3)
				add_triangle(v1, v3, v4, texture_index,rPoint1,rPoint3,rPoint4)
			else:
				## Možnost 2: Úhlopříčka V2 -> V4
				add_triangle(v2, v3, v4, texture_index,rPoint2,rPoint3,rPoint4)
				add_triangle(v2, v4, v1, texture_index,rPoint2,rPoint4,rPoint1)

	# Dokončení sítě
	surface_tool.generate_normals()
	surface_tool.index() 

	mesh_instance.mesh = surface_tool.commit()

## --- POMOCNÁ FUNKCE ---

# Přidá trojúhelník do SurfaceTool s nastavením UV mapování
func add_triangle(p1: Vector3, p2: Vector3, p3: Vector3, texture_index: int,uv1:Vector2,uv2:Vector2,uv3:Vector2):
	# UV mapování je zde zjednodušeno pro každý trojúhelník na rohy (0,0), (1,0), (0,1).
	# Ve Fragment Shaderu se toto UV použije pro mapování na oblast v Texture Atlasu.
	
	# UV mapování v rámci jednoho čtverce:
	#var uv_p1 = Vector2(0.0, 0.0) 
	#var uv_p2 = Vector2(1.0, 0.0)
	#var uv_p3 = Vector2(0.0, 1.0)
	var uv_p1 = Vector2(uv1.x, uv1.y) 
	var uv_p2 = Vector2(uv2.x, uv2.y)
	var uv_p3 = Vector2(uv3.x, uv3.y)
	
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
