extends Node3D

var sprback:Sprite2D
var waiting_for_input = false

func load_custom_texture(path: String) -> ImageTexture:#move to imports
	var img = Image.load_from_file(path)
	if img == null:
		return null        
	var tex = ImageTexture.create_from_image(img)    
	return tex


var fadeNode: Node3D
func fadeInit():
	if(!get_tree().root.get_node_or_null("FadeInOut")):
		var fade_layer_scene = preload("res://scenes/FadeInOut.tscn")
		var new_layer = fade_layer_scene.instantiate()
		get_tree().root.add_child(new_layer)
		new_layer.name = "FadeInOut"
	if(!fadeNode):
		fadeNode=get_tree().root.get_node_or_null("FadeInOut")
func addFadeIn():
	fadeInit()
	fadeNode.start_fade(1.0, Color(0, 0, 0, 0),Color(0, 0, 0, 1))
func addFadeOut():
	fadeInit()
	fadeNode.start_fade(1.0, Color(0, 0, 0, 1),Color(0, 0, 0, 0))

var Main_DecodeLevel
var Main_Sounds
var MainMusic
func _ready():
	await get_tree().process_frame
	sprback = $Control/Sprite2D
	#Engine.max_fps = 60
	Main_DecodeLevel = get_node("DecodeLevel")
	Main_Sounds = $Sounds
	Main_Sounds.MainMusic = get_node("Sounds").get_node("MidiPlayer")
	Main_Sounds.MainMusicHi = get_node("Sounds").get_node("AudioStreamPlayer")
	Main_DecodeLevel.Main_Sounds = Main_Sounds	
	addFadeOut()
	showMyImg(0)
	
var runned = false
var animIndex
	
func showMyImg(index):
	const SPRITE_DIR = "res://convertdata/HSCREEN/"
	var file_name_spr
	if(index==0):
		file_name_spr="welcomeScreen.png"
	var file_path_spr = SPRITE_DIR + file_name_spr
	var tex2 = load_custom_texture(file_path_spr)
	sprback.texture=tex2
	sprback.centered = true
	var screen_size = get_viewport().get_visible_rect().size
	sprback.position = screen_size / 2	
	var tex_size = tex2.get_size()
	var scale_factor = Vector2(screen_size.x / tex_size.x, screen_size.y / tex_size.y)	
	# Pokud chceš zachovat poměr stran a mít černé pruhy (fit):
	# var final_scale = min(scale_factor.x, scale_factor.y)
	# Pokud chceš roztáhnout bez ohledu na deformaci:
	sprback.scale = scale_factor
	sprback.show()
	waiting_for_input = true
	
func _input(event):
	if waiting_for_input:
		if event is InputEventKey or event is InputEventMouseButton:
			if event.is_pressed():
				waiting_for_input = false
				addFadeIn()
				await fadeNode.fade_finished
				addFadeOut()
				playAnim(1)
	
func playAnim(index:int):
	animIndex=index
	runned = true
	animInit()

var animWidth = 320
var animHeight = 200
var animImage: Image
var animTextureRect: ImageTexture

func animInit():
	animImage = Image.create_empty(animWidth, animHeight, false, Image.FORMAT_RGB8)
	animTextureRect = ImageTexture.create_from_image(animImage)
	sprback.texture = animTextureRect
	Main_DecodeLevel.init()
	Main_DecodeLevel.initSound()
	match animIndex:
		0:
			Main_Sounds.setSoundBank(3)
		1:
			Main_Sounds.setSoundBank(4)
	Main_DecodeLevel.playAnim(animIndex)
	
func endAnim():
	runned=false
	addFadeIn()
	await fadeNode.fade_finished
	get_tree().change_scene_to_file(Global.last_scene_path)
	
func _process(_p_delta) -> void:
	if(!runned):
		return
	var is_skipping = Input.is_anything_pressed() or \
					  Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) or \
					  Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)
	var endAnim
	if(is_skipping):
		endAnim = Main_DecodeLevel.playAnimStep(1)
		Main_Sounds.stopAllSounds()
		endAnim()
	else:
		endAnim = Main_DecodeLevel.playAnimStep(0)
	if(endAnim):
		endAnim()
	else:
		animImage.set_data(animWidth, animHeight, false, Image.FORMAT_RGB8, Main_DecodeLevel.getVGABuffer())
		animTextureRect.update(animImage)
	
