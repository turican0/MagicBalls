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
	#initialize_grid_data()
	
	# 3. Generování a vykreslení sítě
	get_parent().get_node("DecodeLevel").init()
	#recalculate_mesh()	
	#for y in range(GRID_SIZE):
		#for x in range(GRID_SIZE):
			#texture_indices[x][y]=get_parent().get_node("DecodeLevel").mapTerrainType_10B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]
	#for y in range(VERTEX_COUNT):
		#for x in range(VERTEX_COUNT):
			#vertices[x][y].y=get_parent().get_node("DecodeLevel").mapHeightmap_11B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]*0.125
	#recalculate_mesh()
	
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
	for x in [-2, -1, 0, 1, 2]:
		for z in [-2, -1, 0, 1, 2]:
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

var wave_scale: Array = []

func recalculate_mesh():
	surface_tool = SurfaceTool.new()
	surface_tool.begin(Mesh.PRIMITIVE_TRIANGLES)
	
	wave_scale.resize(GRID_SIZE)
	for x in range(GRID_SIZE):
		wave_scale[x] = []
		wave_scale[x].resize(GRID_SIZE)
		for y in range(GRID_SIZE):
			wave_scale[x][y] = 0
			if(texture_indices[x][y]==0)&&(texture_indices[(x+(GRID_SIZE-1))%GRID_SIZE][y]==0)&&(texture_indices[x][(y+(GRID_SIZE-1))%GRID_SIZE]==0)&&(texture_indices[(x+(GRID_SIZE-1))%GRID_SIZE][(y+(GRID_SIZE-1))%GRID_SIZE]==0):
				wave_scale[x][y] = 1
				
	
	for x in range(GRID_SIZE):
		for y in range(GRID_SIZE):
			var textUV_42:int = ((get_parent().get_node("DecodeLevel").mapAngle_13B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)] >> 2) & 0x1C)
			var rPoint1 = Vector2(uv_table_d4350[textUV_42][0],uv_table_d4350[textUV_42][1])
			var rPoint2 = Vector2(uv_table_d4350[textUV_42][2],uv_table_d4350[textUV_42][3])
			var rPoint3 = Vector2(uv_table_d4350[textUV_42][4],uv_table_d4350[textUV_42][5])
			var rPoint4 = Vector2(uv_table_d4350[textUV_42][6],uv_table_d4350[textUV_42][7])
			var v1 = vertices[x][y]
			var v2 = vertices[x+1][y]
			var v3 = vertices[x+1][y+1]
			var v4 = vertices[x][y+1]
			
			var waves1 = wave_scale[x][y]
			var waves2 = wave_scale[(x+1)%GRID_SIZE][y]
			var waves3 = wave_scale[(x+1)%GRID_SIZE][(y+1)%GRID_SIZE]
			var waves4 = wave_scale[x][(y+1)%GRID_SIZE]
			
			var texture_index = texture_indices[x][y]
			if ((x+y+1)&1):
				add_triangle(v1, v2, v3, texture_index,texture_index,0,rPoint1,rPoint2,rPoint3,waves1,waves2,waves3,waves1,waves2,waves3)
				add_triangle(v1, v3, v4, texture_index,texture_index,0,rPoint1,rPoint3,rPoint4,waves1,waves3,waves4,waves1,waves3,waves4)
			else:
				add_triangle(v2, v3, v4, texture_index,texture_index,0,rPoint2,rPoint3,rPoint4,waves2,waves3,waves4,waves2,waves3,waves4)
				add_triangle(v2, v4, v1, texture_index,texture_index,0,rPoint2,rPoint4,rPoint1,waves2,waves4,waves1,waves2,waves4,waves1)
	surface_tool.generate_normals()
	surface_tool.index()
	mesh_instance.mesh = surface_tool.commit()

func add_triangle(p1: Vector3, p2: Vector3, p3: Vector3, idx1: int, idx2: int, weight: float, uv1: Vector2, uv2: Vector2, uv3: Vector2, w1_1:float, w1_2:float, w1_3:float, w2_1:float, w2_2:float, w2_3:float):
	var verts = [p1, p2, p3]
	var uvs = [uv1, uv2, uv3]
	
	var wave_sizes1 = [w1_1, w1_2, w1_3]
	var wave_sizes2 = [w2_1, w2_2, w2_3]
	
	for i in range(3):
		surface_tool.set_color(Color(weight, wave_sizes1[i], wave_sizes2[i]))
		surface_tool.set_uv(uvs[i])
		surface_tool.set_uv2(Vector2(idx1, idx2))
		surface_tool.add_vertex(verts[i])

# Funkce, která by se volala, kdykoli se změní výška:
func modify_vertex_height(x: int, y: int, new_height: float):
	if x >= 0 and x < VERTEX_COUNT and y >= 0 and y < VERTEX_COUNT:
		# Změna výšky vrcholu
		vertices[x][y].y = new_height
		
		# Po změně vždy přepočítat celou síť (nebo jen dotčené čtverce)
		# Pro zjednodušení voláme recalculate_mesh() celou:
		recalculate_mesh()

func renew_terrain():
	for y in range(GRID_SIZE):
		for x in range(GRID_SIZE):
			texture_indices[x][y]=get_parent().get_node("DecodeLevel").mapTerrainType_10B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]
	for y in range(VERTEX_COUNT):
		for x in range(VERTEX_COUNT):
			vertices[x][y].y=get_parent().get_node("DecodeLevel").mapHeightmap_11B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]*0.125
	recalculate_mesh()
