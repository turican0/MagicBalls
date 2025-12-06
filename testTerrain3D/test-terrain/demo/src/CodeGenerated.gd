extends Node

var terrain: Terrain3D

@export var green_ta: Terrain3DTextureAsset
@export var brown_ta: Terrain3DTextureAsset
@export var terrain_script_source: Script
@export var grass_particle: PackedScene


func _ready() -> void:
	$UI.player = $Player
		
	if has_node("RunThisSceneLabel3D"):
		$RunThisSceneLabel3D.queue_free()

	#terrain = await create_terrain()
	terrain = await load_terrain("level1")

	# Enable runtime navigation baking using the terrain
	# Enable `Debug/Visible Navigation` if you wish to see it
	$RuntimeNavigationBaker.terrain = terrain
	$RuntimeNavigationBaker.enabled = true

func load_terrain(level_name: String) -> Terrain3D:
	var grass_ma: Terrain3DMeshAsset = create_mesh_asset("Grass", Color.from_hsv(120./360., .4, .37)) 

	# Create a terrain
	terrain = Terrain3D.new()
	terrain.name = "Terrain3D"
	terrain.set_script(terrain_script_source)
	add_child(terrain, true)

	# Set material and assets
	terrain.material.world_background = Terrain3DMaterial.NONE
	terrain.material.auto_shader = true
	terrain.material.set_shader_param("auto_slope", 10)
	terrain.material.set_shader_param("blend_sharpness", .975)
	terrain.assets = Terrain3DAssets.new()
	terrain.assets.set_texture(0, green_ta)
	terrain.assets.set_texture(1, brown_ta)
	terrain.assets.set_mesh_asset(0, grass_ma)

	var image_path_heightmap = "res://levels/"+level_name+"/heightmap.png"
	var image_heightmap = Image.load_from_file(image_path_heightmap)
	image_heightmap.convert(Image.FORMAT_RGB8)
	var img_width_heightmap = image_heightmap.get_width()
	var img_height_heightmap = image_heightmap.get_height()
	#print("Načten obrázek s rozměry: ", img_width, "x", img_height)
	var img_heightmap: Image = Image.create_empty(img_width_heightmap, img_height_heightmap, false, Image.FORMAT_RF)
	for y in range(img_height_heightmap):
		for x in range(img_width_heightmap):
			var color_heightmap = image_heightmap.get_pixel(x, y)
			var red_value_heightmap = color_heightmap.r
			var red_value_255_heightmap = round(red_value_heightmap * 255)
			img_heightmap.set_pixel(x, y, Color(red_value_255_heightmap/(256*10), 0., 0., 1.))
			#print("Pixel [", x, ", ", y, "] - Red (0-255): ", red_value_255, ", Red (0.0-1.0): ", red_value)
	terrain.region_size = Terrain3D.SIZE_64
	terrain.data.import_images([img_heightmap, null, null], Vector3(-64, 0, -64), 0.0, 150.0)

	var new_level_instance = grass_particle.instantiate()
	terrain.add_child(new_level_instance)

	#var new_level_instance = grass_particle.instantiate()
	#new_level_instance.terrain = terrain
	
	# Generate height map w/ 32-bit noise and import it with scale
	#var noise := FastNoiseLite.new()
	#noise.frequency = 0.0005
	#var img: Image = Image.create_empty(2048, 2048, false, Image.FORMAT_RF)
	#for x in img.get_width():
		#for y in img.get_height():
			#img.set_pixel(x, y, Color(noise.get_noise_2d(x, y), 0., 0., 1.))
	#terrain.region_size = Terrain3D.SIZE_1024
	#terrain.data.import_images([img, null, null], Vector3(-1024, 0, -1024), 0.0, 150.0)

	# Instance foliage
	var image_path_grass = "res://levels/"+level_name+"/grass.png"
	var image_grass = Image.load_from_file(image_path_grass)
	image_grass.convert(Image.FORMAT_RGB8)
	var img_width_grass = image_grass.get_width()
	var img_height_grass = image_grass.get_height()
	var xforms: Array[Transform3D]
	var width_grass: int = 100
	for y in range(img_height_grass):
		for x in range(img_width_grass):
			var color_grass = image_grass.get_pixel(x, y)
			var red_value_grass = color_grass.r
			var red_value_255_grass = round(red_value_grass * 255)
			if(red_value_255_grass>0):
				var pos := Vector3(x, 0, y) - Vector3(width_grass, 0, width_grass) * .5
				pos.y = terrain.data.get_height(pos)
				xforms.push_back(Transform3D(Basis(), pos))
	terrain.instancer.add_transforms(0, xforms)
	
	#var xforms: Array[Transform3D]
	#var width: int = 100
	#var step: int = 2
	#for x in range(0, width, step):
		#for z in range(0, width, step):
			#var pos := Vector3(x, 0, z) - Vector3(width, 0, width) * .5
			#pos.y = terrain.data.get_height(pos)
			#xforms.push_back(Transform3D(Basis(), pos))
	#terrain.instancer.add_transforms(0, xforms)

	# Enable the next line and `Debug/Visible Collision Shapes` to see collision
	#terrain.collision.mode = Terrain3DCollision.DYNAMIC_EDITOR

	return terrain

func create_terrain() -> Terrain3D:
	# Create textures
	var green_gr := Gradient.new()
	green_gr.set_color(0, Color.from_hsv(100./360., .35, .3))
	green_gr.set_color(1, Color.from_hsv(120./360., .4, .37))
	var green_tax: Terrain3DTextureAsset = await create_texture_asset("Grass", green_gr, 1024)
	green_tax.uv_scale = 0.1
	green_tax.detiling_rotation = 0.1

	var brown_gr := Gradient.new()
	brown_gr.set_color(0, Color.from_hsv(30./360., .4, .3))
	brown_gr.set_color(1, Color.from_hsv(30./360., .4, .4))
	var brown_tax: Terrain3DTextureAsset = await create_texture_asset("Dirt", brown_gr, 1024)
	brown_tax.uv_scale = 0.03
	brown_tax.detiling_rotation = 0.1
	
	var grass_ma: Terrain3DMeshAsset = create_mesh_asset("Grass", Color.from_hsv(120./360., .4, .37)) 

	# Create a terrain
	terrain = Terrain3D.new()
	terrain.name = "Terrain3D"
	terrain.set_script(terrain_script_source)
	add_child(terrain, true)

	# Set material and assets
	terrain.material.world_background = Terrain3DMaterial.NONE
	terrain.material.auto_shader = true
	terrain.material.set_shader_param("auto_slope", 10)
	terrain.material.set_shader_param("blend_sharpness", .975)
	terrain.assets = Terrain3DAssets.new()
	terrain.assets.set_texture(0, green_ta)
	terrain.assets.set_texture(1, brown_ta)
	terrain.assets.set_mesh_asset(0, grass_ma)

	# Generate height map w/ 32-bit noise and import it with scale
	var noise := FastNoiseLite.new()
	noise.frequency = 0.0005
	var img: Image = Image.create_empty(2048, 2048, false, Image.FORMAT_RF)
	for x in img.get_width():
		for y in img.get_height():
			img.set_pixel(x, y, Color(noise.get_noise_2d(x, y), 0., 0., 1.))
	terrain.region_size = Terrain3D.SIZE_1024
	terrain.data.import_images([img, null, null], Vector3(-1024, 0, -1024), 0.0, 150.0)

	# Instance foliage
	var xforms: Array[Transform3D]
	var width: int = 100
	var step: int = 2
	for x in range(0, width, step):
		for z in range(0, width, step):
			var pos := Vector3(x, 0, z) - Vector3(width, 0, width) * .5
			pos.y = terrain.data.get_height(pos)
			xforms.push_back(Transform3D(Basis(), pos))
	terrain.instancer.add_transforms(0, xforms)

	# Enable the next line and `Debug/Visible Collision Shapes` to see collision
	#terrain.collision.mode = Terrain3DCollision.DYNAMIC_EDITOR

	return terrain


func create_texture_asset(asset_name: String, gradient: Gradient, texture_size: int = 512) -> Terrain3DTextureAsset:
	# Create noise map
	var fnl := FastNoiseLite.new()
	fnl.frequency = 0.004
	
	# Create albedo noise texture
	var alb_noise_tex := NoiseTexture2D.new()
	alb_noise_tex.width = texture_size
	alb_noise_tex.height = texture_size
	alb_noise_tex.seamless = true
	alb_noise_tex.noise = fnl
	alb_noise_tex.color_ramp = gradient
	await alb_noise_tex.changed
	var alb_noise_img: Image = alb_noise_tex.get_image()

	# Create albedo + height texture
	for x in alb_noise_img.get_width():
		for y in alb_noise_img.get_height():
			var clr: Color = alb_noise_img.get_pixel(x, y)
			clr.a = clr.v # Noise as height
			alb_noise_img.set_pixel(x, y, clr)
	alb_noise_img.generate_mipmaps()
	var albedo := ImageTexture.create_from_image(alb_noise_img)

	# Create normal + rough texture
	var nrm_noise_tex := NoiseTexture2D.new()
	nrm_noise_tex.width = texture_size
	nrm_noise_tex.height = texture_size
	nrm_noise_tex.as_normal_map = true
	nrm_noise_tex.seamless = true
	nrm_noise_tex.noise = fnl
	await nrm_noise_tex.changed
	var nrm_noise_img = nrm_noise_tex.get_image()
	for x in nrm_noise_img.get_width():
		for y in nrm_noise_img.get_height():
			var normal_rgh: Color = nrm_noise_img.get_pixel(x, y)
			normal_rgh.a = 0.8 # Roughness
			nrm_noise_img.set_pixel(x, y, normal_rgh)
	nrm_noise_img.generate_mipmaps()
	var normal := ImageTexture.create_from_image(nrm_noise_img)

	var ta := Terrain3DTextureAsset.new()
	ta.name = asset_name
	ta.albedo_texture = albedo
	ta.normal_texture = normal
	return ta


func create_mesh_asset(asset_name: String, color: Color) -> Terrain3DMeshAsset:
	var ma := Terrain3DMeshAsset.new()
	ma.name = asset_name
	ma.generated_type = Terrain3DMeshAsset.TYPE_TEXTURE_CARD
	ma.material_override.albedo_color = color
	return ma
