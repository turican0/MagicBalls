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
	#initialize_grid_data()
	
	# 3. Generování a vykreslení sítě
	get_parent().get_node("DecodeLevel").init()
	#recalculate_mesh()	
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
var material

func initialize_nodes():
	mesh_instance = MeshInstance3D.new()
	mesh_instance.name = "TerrainMesh"
	add_child(mesh_instance)
	
	# Zde načtěte váš ShaderMaterial (který odkazuje na Texture Atlas)
	material = load("res://terrainMB/terrain_material.tres")
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
	for y in range(GRID_SIZE):
		for x in range(GRID_SIZE):
			texture_indices[x][y]=get_parent().get_node("DecodeLevel").mapTerrainType_10B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]
	for y in range(VERTEX_COUNT):
		for x in range(VERTEX_COUNT):
			vertices[x][y].y=get_parent().get_node("DecodeLevel").mapHeightmap_11B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]*0.125
			
	surface_tool = SurfaceTool.new()
	surface_tool.begin(Mesh.PRIMITIVE_TRIANGLES)
	
	surface_tool.set_custom_format(0, SurfaceTool.CUSTOM_RGBA_FLOAT)
	
	for x in range(GRID_SIZE):
		for y in range(GRID_SIZE):
			var rPoint1 = Vector2(0,0)
			var rPoint2 = Vector2(1,0)
			var rPoint3 = Vector2(1,1)
			var rPoint4 = Vector2(0,1)
			var v1 = vertices[x][y]
			var v2 = vertices[x+1][y]
			var v3 = vertices[x+1][y+1]
			var v4 = vertices[x][y+1]
			
			var g1 = Vector2(x, y)
			var g2 = Vector2(x+1, y)
			var g3 = Vector2(x+1, y+1)
			var g4 = Vector2(x, y+1)
			
			if ((x+y+1)&1):
				add_triangle(v1, v2, v3, rPoint1,rPoint2,rPoint3,g1, g2, g3, g1)
				add_triangle(v1, v3, v4, rPoint1,rPoint3,rPoint4,g1, g3, g4, g1)
			else:
				add_triangle(v2, v3, v4, rPoint2,rPoint3,rPoint4,g2, g3, g4, g1)
				add_triangle(v2, v4, v1, rPoint2,rPoint4,rPoint1,g2, g4, g1, g1)
	surface_tool.generate_normals()
	surface_tool.index()
	mesh_instance.mesh = surface_tool.commit()

func add_triangle(p1: Vector3, p2: Vector3, p3: Vector3, uv1: Vector2, uv2: Vector2, uv3: Vector2, grid_p1: Vector2, grid_p2: Vector2, grid_p3: Vector2, main_p: Vector2):
	var verts = [p1, p2, p3]
	var uvs = [uv1, uv2, uv3]
	
	var global_uvs = [
		grid_p1 / float(GRID_SIZE),
		grid_p2 / float(GRID_SIZE),
		grid_p3 / float(GRID_SIZE)
	]
	
	var main_uvs = main_p / float(GRID_SIZE)
	
	for i in range(3):
		surface_tool.set_uv(uvs[i])
		surface_tool.set_custom(0, Color(global_uvs[i].x, global_uvs[i].y, main_uvs.x, main_uvs.y))
		surface_tool.add_vertex(verts[i])

# Funkce, která by se volala, kdykoli se změní výška:
func modify_vertex_height(x: int, y: int, new_height: float):
	if x >= 0 and x < VERTEX_COUNT and y >= 0 and y < VERTEX_COUNT:
		# Změna výšky vrcholu
		vertices[x][y].y = new_height
		
		# Po změně vždy přepočítat celou síť (nebo jen dotčené čtverce)
		# Pro zjednodušení voláme recalculate_mesh() celou:
		recalculate_mesh()

var height_data : PackedFloat32Array = []
var height_image : Image
var height_texture : ImageTexture

var control_data : PackedByteArray = PackedByteArray()
var control_image : Image
var control_texture : ImageTexture

func initialize_controlmap():
	control_image = Image.create(GRID_SIZE, GRID_SIZE, false, Image.FORMAT_RGBA8)
	control_texture = ImageTexture.create_from_image(control_image)
	var mat = mesh_instance.material_override as ShaderMaterial
	if mat:
		mat.set_shader_parameter("control_map", control_texture)

func initialize_heightmap():
	height_data.resize(GRID_SIZE * GRID_SIZE)
	height_data.fill(0.0)
	height_image = Image.create(GRID_SIZE, GRID_SIZE, false, Image.FORMAT_RF)
	height_texture = ImageTexture.create_from_image(height_image)
	var mat = mesh_instance.material_override as ShaderMaterial
	if mat:
		mat.set_shader_parameter("height_map", height_texture)

func renew_terrain():
	if not height_image:
		initialize_heightmap()
	if not control_image:
		initialize_controlmap()
	var decode = get_parent().get_node("DecodeLevel")
	# Zajištění velikosti: GRID_SIZE * GRID_SIZE * 4 (RGBA)
	if control_data.size() != GRID_SIZE * GRID_SIZE * 4:
		control_data.resize(GRID_SIZE * GRID_SIZE * 4)
	
	var raw_h_map = decode.mapHeightmap_11B4E0
	var raw_t_map = decode.mapTerrainType_10B4E0
	for y in range(GRID_SIZE):
		for x in range(GRID_SIZE):
			var final_c=raw_t_map[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]
			texture_indices[x][y]=final_c
			var textUV_42:int = ((decode.mapAngle_13B4E0[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)] >> 2) & 0x1C)
			control_data[(y * GRID_SIZE + x)*4 + 0] = final_c
			control_data[(y * GRID_SIZE + x)*4 + 1] = textUV_42
			control_data[(y * GRID_SIZE + x)*4 + 2] = 0
			control_data[(y * GRID_SIZE + x)*4 + 3] = 0
	for y in range(VERTEX_COUNT):
		for x in range(VERTEX_COUNT):
			var raw_h=raw_h_map[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]
			var final_h = raw_h * 0.125
			vertices[x][y].y = final_h
			
	for y in range(GRID_SIZE):
		for x in range(GRID_SIZE):
			var raw_h=raw_h_map[(y % GRID_SIZE) * GRID_SIZE + (x % GRID_SIZE)]
			var final_h = raw_h * 0.125
			height_data[y * GRID_SIZE + x] = final_h
	update_gpu_heightmap()
	update_gpu_controlmap()
	#recalculate_mesh()
	
func update_gpu_heightmap():
	if not height_image:
		initialize_heightmap() # Zavolá Image.create a ImageTexture.create
	var byte_array = height_data.to_byte_array()
	height_image.set_data(GRID_SIZE, GRID_SIZE, false, Image.FORMAT_RF, byte_array)
	height_texture.update(height_image)
	
func update_gpu_controlmap():
	control_image.set_data(GRID_SIZE, GRID_SIZE, false, Image.FORMAT_RGBA8, control_data)
	control_texture.update(control_image)

func initialize_control_map():
	control_data.resize(GRID_SIZE * GRID_SIZE * 4) # RGBA (4 bajty na pixel)
	control_image = Image.create(GRID_SIZE, GRID_SIZE, false, Image.FORMAT_RGBA8)
	control_texture = ImageTexture.create_from_image(control_image)

	var mat = mesh_instance.material_override as ShaderMaterial
	mat.set_shader_parameter("control_map", control_texture)
	
func update_control_mapxx():
	var decode = get_parent().get_node("DecodeLevel")
	for y in range(GRID_SIZE):
		for x in range(GRID_SIZE):
			var idx = y * GRID_SIZE + x
			var t_idx = decode.mapTerrainType_10B4E0[idx]
			# Zapíšeme do bajtů (RGBA)
			control_data[idx * 4 + 0] = t_idx # R: Index textury
			control_data[idx * 4 + 1] = 0     # G: (rezerva)
			control_data[idx * 4 + 2] = 0     # B: (rezerva)
			control_data[idx * 4 + 3] = 255   # A: (rezerva)
			
	control_image.set_data(GRID_SIZE, GRID_SIZE, false, Image.FORMAT_RGBA8, control_data)
	control_texture.update(control_image)
