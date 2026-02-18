extends Node3D

@onready var foreground = $Control/Foreground

var waiting_for_input = false

signal video_finished

var fadeNode: Node3D

var Main_DecodeLevel
#var Main_Sounds
#var MainMusic
func _ready():
	await get_tree().process_frame
	Engine.max_fps = 25
	Main_DecodeLevel = get_node("DecodeLevel")
	#Global.initSound()
	fadeNode = Global.addFadeOut(fadeNode)
	showMyImg(0)

var runned = false
var animIndex

func showMyImg(index):
	var SPRITE_DIR = Global.convertdata+"HSCREEN/"
	var file_name_spr
	if(index==0):
		file_name_spr="welcomeScreen.png"
	var file_path_spr = SPRITE_DIR + file_name_spr
	var tex2 = Global.load_custom_texture(file_path_spr)
	foreground.texture=tex2
	waiting_for_input = true

func _input(event):
	if waiting_for_input:
		if event is InputEventKey or event is InputEventMouseButton:
			if event.is_pressed():
				waiting_for_input = false
				fadeNode=Global.addFadeIn(fadeNode)
				await fadeNode.fade_finished
				fadeNode=Global.addFadeOut(fadeNode)
				playAnim(1)
				await video_finished
				playAnim(2)
				await video_finished
				runned=false
				fadeNode = Global.addFadeIn(fadeNode)
				await fadeNode.fade_finished
				get_tree().change_scene_to_file(Global.last_scene_path)

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
	foreground.texture = animTextureRect
	Main_DecodeLevel.init()
	Global.initSound()
	match animIndex:
		0:
			Global.Main_Sounds.setSoundBank(3)
		1:
			Global.Main_Sounds.setSoundBank(4)
		2:
			Global.Main_Sounds.setSoundBank(4)
	Main_DecodeLevel.anim1Begin($Control/Foreground,animIndex)

func endAnim():
	Main_DecodeLevel.anim1End()
	emit_signal("video_finished")

func _process(_p_delta) -> void:
	if(!runned):
		return
	#var is_skipping = Input.is_anything_pressed() or \
					  #Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) or \
					  #Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)
	var endAnimVar
	#if(is_skipping):
	endAnimVar = Main_DecodeLevel.anim1Step()
		#Global.Main_Sounds.stopAllSounds()
		#endAnim()
	#else:
		#endAnim = Main_DecodeLevel.anim1Step(0)
	if(endAnimVar):
		Global.Main_Sounds.stopAllSounds()
		endAnim()
	else:
		animImage.set_data(animWidth, animHeight, false, Image.FORMAT_RGB8, Main_DecodeLevel.getVGABuffer())
		animTextureRect.update(animImage)
