@tool  # Toto magické slovo aktivuje běh skriptu v editoru
extends MeshInstance3D

func _process(_delta):
	# V editoru chceme vidět změny v reálném čase, 
	# ale pro výkon je lepší generovat mesh jen při změně nebo jednou.
	if mesh == null:
		update_star()

func update_star():
	mesh = create_star_mesh()

func create_star_mesh() -> ArrayMesh:
	var vertices = PackedVector3Array()
	var outer_radius = 2.0
	var inner_radius = 0.8
	var thickness = 0.4
	var points = 5

	# Geometrie bodů
	for i in range(points * 2):
		var radius = outer_radius if i % 2 == 0 else inner_radius
		var angle = deg_to_rad(i * 360.0 / (points * 2))
		vertices.append(Vector3(cos(angle) * radius, sin(angle) * radius, 0))
	
	var front_center_idx = vertices.size()
	vertices.append(Vector3(0, 0, thickness))
	var back_center_idx = vertices.size()
	vertices.append(Vector3(0, 0, -thickness))

	# Indexy trojúhelníků
	var indices = PackedInt32Array()
	for i in range(points * 2):
		var next = (i + 1) % (points * 2)
		# Přední strana
		indices.append(i)
		indices.append(next)
		indices.append(front_center_idx)
		# Zadní strana
		indices.append(next)
		indices.append(i)
		indices.append(back_center_idx)

	var arr = []
	arr.resize(Mesh.ARRAY_MAX)
	arr[Mesh.ARRAY_VERTEX] = vertices
	arr[Mesh.ARRAY_INDEX] = indices

	var array_mesh = ArrayMesh.new()
	array_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arr)
	
	# Vygenerování normál pro správné stínování
	var st = SurfaceTool.new()
	st.create_from(array_mesh, 0)
	st.generate_normals()
	return st.commit()
