extends SceneTree

# Screenshots of the launcher and the settings in several languages - to check the translations.
# Copied to godot-code/zz_tools, run with a window: Godot --path godot-code -s res://zz_tools/settingsshot.gd
# MB_SHOT_DIR = folder for the PNGs.
const LANGS := [2, 7, 3, 8, 6] # en, cs, de, pl, sv
var _dir := OS.get_environment("MB_SHOT_DIR")

func _initialize() -> void:
	_run.call_deferred()

func _shot(name: String) -> void:
	for i in 4:
		await process_frame
	await RenderingServer.frame_post_draw
	root.get_texture().get_image().save_png(_dir.path_join(name + ".png"))

func _run() -> void:
	root.size = Vector2i(1280, 720)
	var settings = load("res://scenes/Settings.tscn").instantiate()
	root.add_child(settings)
	for i in 3:
		await process_frame
	settings._countdown_active = false # the countdown would start the game
	var g = root.get_node("Global")
	g.set_text_language(7)
	await _shot("launcher-cs")
	settings._open_settings()
	var tabs: TabContainer = settings._settings_canvas.find_children("*", "TabContainer", true, false)[0]
	for lang in LANGS:
		g.set_text_language(lang)
		for tab in [0, 1, 3, 4]:
			tabs.current_tab = tab
			await _shot("settings-%s-%d" % [g.LANG_LOCALES[lang - 1], tab])
	quit()
