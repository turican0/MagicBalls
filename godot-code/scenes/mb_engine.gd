extends Node3D

var Main_DecodeLevel

var runned:bool = false

func init():
	Global.loadScreenInit()
	Main_DecodeLevel.MBEXinit()
	Global.initSound()
	#Main_DecodeLevel.anim1Begin($Control/Foreground)
	Main_DecodeLevel.setLoadScreenBuffer()
	Main_DecodeLevel.changeLanguage(2)
#	Global.Main_Sounds.setSoundBank(3)
	runned=true

func endAnim():
	#Main_DecodeLevel.anim1End()
	emit_signal("video_finished")

#var animWidth = 320
#var animHeight = 200
#var animImage: Image
#var animTextureRect: ImageTexture
func _process(_p_delta) -> void:
	if(!runned):
		return
	var endAnimVar
	endAnimVar = Main_DecodeLevel.MBrun()
	if(endAnimVar):
		Global.Main_Sounds.stopAllSounds()
		endAnim()
	#else:
		#animImage.set_data(animWidth, animHeight, false, Image.FORMAT_RGB8, Main_DecodeLevel.getVGABuffer())
		#animTextureRect.update(animImage)
	
