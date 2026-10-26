extends Node3D

# =============================================
# UI THEME SETTINGS
# =============================================

const UI_BG_COLOR          = Color(0.08, 0.08, 0.12)
const UI_BORDER_COLOR      = Color(0.30, 0.30, 0.50)
const UI_BORDER_WIDTH      = 2
const UI_CORNER_RADIUS     = 6

const UI_TITLE_COLOR       = Color(1.00, 1.00, 0.60)
const UI_TITLE_FONT_SIZE   = 18

const UI_TEXT_COLOR        = Color(0.90, 0.90, 0.90)
const UI_TEXT_SELECT_COLOR = Color(1.00, 1.00, 0.60)

const UI_BTN_NORMAL_COLOR  = Color(0.15, 0.15, 0.30)
const UI_BTN_HOVER_COLOR   = Color(0.25, 0.25, 0.50)
const UI_BTN_PRESS_COLOR   = Color(0.10, 0.10, 0.25)
const UI_BTN_BORDER_COLOR  = Color(0.30, 0.30, 0.60)
const UI_BTN_HOVER_BORDER  = Color(0.50, 0.50, 0.80)
const UI_BTN_TEXT_COLOR    = Color(0.90, 0.90, 0.90)

const UI_TREE_BG_COLOR     = Color(0.05, 0.05, 0.10)
const UI_TREE_BORDER_COLOR = Color(0.20, 0.20, 0.40)
const UI_TREE_TEXT_COLOR   = Color(0.85, 0.85, 0.85)
const UI_TREE_SELECT_COLOR = Color(0.20, 0.20, 0.50)

const UI_LOADING_BG_COLOR  = Color(0, 0, 0, 1)
const UI_SPINNER_COLOR     = Color(0.40, 0.80, 1.00)
const UI_COUNTDOWN_COLOR   = Color(1.00, 0.40, 0.40)

# =============================================
# CONFIG
# =============================================

const CONFIG_PATH       = "user://config.cfg"
const COUNTDOWN_SECONDS = 3
const EDITOR_KEY        = KEY_E
const SETTINGS_KEY      = KEY_S
const REIMPORT_KEY      = KEY_R
const CREDITS_KEY       = KEY_C
# made by tools/sketchfab_credits.py (models) and by hand (the rest), also the source of CREDITS.md
const MODELS_CREDITS_PATH = "res://credits/models_credits.json"
const OTHER_CREDITS_PATH  = "res://credits/other_credits.json"

# =============================================
# DEFAULT SETTINGS
# =============================================

const DEFAULTS = {
	"video": {
		"resolution_index": 3,   # 1280x720
		"display_mode":     1,   # 0=Windowed 1=Fullscreen 2=Borderless
		#"vsync":            0,   # 0=On 1=Off 2=Adaptive
		"texture_quality":  2,   # 0=Low 1=Medium 2=High
		#"view_distance":    7,   # 1..10
		"bilinear_filtering": 1,  # 0=Nearest (pixelated) 1=Linear (bilinear, smooth)
		"separate_render_thread": 0, # 0=Off 1=On (Godot's experimental render thread, after a restart; taken from the project setting)
		# graphics quality: a preset (0=Ultra 1=High 2=Medium 3=Low 4=Custom) sets the ones below
		"quality_preset":     0,
		"shadow_quality":     3,  # 0=Off 1=Low 2=Medium 3=High
		"reflections":        1,  # 0=Off 1=On
		"render_scale":       0,  # index into Global.RENDER_SCALES (100 % 85 % 70 % 50 %)
		"force_vertex_shader": 0, # 0=Off 1=On (per-vertex lighting for low-end GPUs, after a restart; taken from the project setting)
	},
	"audio": {
		"master_volume":    100,
		"music_volume":     100,
		"sfx_volume":       100,
		"speech_volume":    100,
		# Same music options as the sound dialog of remc2-configurator (config.json "sound")
		"music_type":       0,    # 0=OGG 1=MIDI (SoundFont) 2=AWE32 emulation
		"mc1_music":        0,    # 0=Off 1=On (AWE32 only: Magic Carpet 1 music in the levels)
		"awe32_gm_intro":   1,    # 0=Off 1=On (AWE32 only: General MIDI music in the intro and cutscenes)
		"awe32_buffer_ms":  200,  # 100..2000, how far ahead the AWE32 emulation renders
	},
	"input": {
		"mouse_sensitivity": 8,
		"invert_y":          1#,  # 0=No 1=Yes
		#"gamepad":           0,  # 0=Auto 1=Always 2=Never
	},
		"game": {
		"level_mode":   0,
		"custom_level": 0,
		"fps_limit":    1,   # 0=20 1=24 2=30 3=40 4=50
		"show_navigation": 0,  # 0=Off 1=On
		"current_fog_index": 7,  # index do Global.fog_presets; rozsah 0..Global.fog_presets.size()-1
	},
	# Same options as the multiplayer dialog of remc2-configurator (MC2 HD mod)
	"multiplayer": {
		"mode":        0,            # 0=Off (single player) 1=Host 2=Join
		"server_ip":   "127.0.0.1",  # Join: address of the host
		"server_port": 3030,         # Host: own port / Join: port of the host
		"client_port": 3031,         # Join: own port
		"debug":       0,            # 0=Off 1=On (--network_debug)
		"record_file": "",           # Host: record the session to this file
	},
}

const RESOLUTIONS = [
	Vector2i(640,  480),
	Vector2i(800,  600),
	Vector2i(1280, 720),
	Vector2i(1920, 1080),
	Vector2i(2560, 1440),
	Vector2i(3840, 2160),
	Vector2i(0, 0),
]

# Ultra (the full detail), High, Medium, Low: the values of QUALITY_KEYS
const QUALITY_KEYS = ["shadow_quality", "reflections", "render_scale", "force_vertex_shader"]
const QUALITY_PRESETS = [
	[3, 1, 0, 0],
	[2, 1, 0, 0],
	[1, 1, 1, 0],
	[0, 0, 2, 1],
]
const QUALITY_CUSTOM = 4

const RESOLUTION_NAMES = [
	"640 × 480",
	"800 × 600",
	"1280 × 720",
	"1920 × 1080",
	"2560 × 1440",
	"3840 × 2160",
	"RES_NATIVE",
]

# =============================================
# STATE
# =============================================

var _cfg: ConfigFile
var _settings: Dictionary = {}

var _countdown_canvas:  CanvasLayer
var _countdown_label:   Label
var _countdown_number:  Label
var _countdown_timer:   float = 0.0
var _countdown_active:  bool  = false

var _settings_canvas:   CanvasLayer
var _settings_open:     bool = false

var _credits_canvas:    CanvasLayer
var _credits_open:      bool = false

# Control refs (filled during UI build)
var _sel_resolution:    OptionButton
var _sel_display:       OptionButton
var _sel_vsync:         OptionButton
var _sel_texture:       OptionButton
var _sl_view_dist:      HSlider
var _sel_fps:           OptionButton
var _sel_show_navigation: OptionButton
var _sel_fog_index:     OptionButton

# Quality controls: the preset, then one per QUALITY_KEYS
var _sel_quality_preset: OptionButton
var _sel_quality: Array[OptionButton] = []

# Rendering controls
var _sel_bilinear_filter: OptionButton
var _sel_render_thread: OptionButton

var _sl_master:         HSlider
var _sl_music:          HSlider
var _sl_sfx:            HSlider
var _sl_speech:         HSlider
var _sel_music_type:    OptionButton
var _sel_mc1_music:     OptionButton
var _sel_awe32_gm_intro: OptionButton
var _sp_awe32_buffer:   SpinBox

var _sl_sensitivity:    HSlider
var _sel_invert_y:      OptionButton
var _sel_gamepad:       OptionButton

var _sel_language:      OptionButton
var _language_before:   int # to go back to on Cancel - the choice shows at once
var _sel_difficulty:    OptionButton
var _sel_autosave:      OptionButton

var _sel_level_mode:    OptionButton
var _sel_custom_level:  OptionButton

var _sel_mp_mode:        OptionButton
var _le_mp_server_ip:    LineEdit
var _sp_mp_server_port:  SpinBox
var _sp_mp_client_port:  SpinBox
var _sel_mp_debug:       OptionButton
var _le_mp_record_file:  LineEdit

# =============================================
# READY
# =============================================

func _ready() -> void:
	MBDebug.log("settings screen")
	Global.apply_game_language()
	_load_config()
	_show_countdown()
	await get_tree().process_frame
	await get_tree().process_frame
	_apply_settings()

func _input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed and not event.echo:
		if _settings_open:
			if event.keycode == KEY_ESCAPE:
				_on_cancel_pressed()
			return
		if _credits_open:
			if event.keycode == KEY_ESCAPE or event.keycode == CREDITS_KEY:
				_close_credits()
			return
		if event.keycode == KEY_ESCAPE:
			_countdown_active = false
			_launch_game()
		elif event.keycode == SETTINGS_KEY:
			_open_settings()
		elif event.keycode == CREDITS_KEY:
			_open_credits()
		elif event.keycode == REIMPORT_KEY:
			_countdown_active = false
			_reimport_scene()
		elif event.keycode == EDITOR_KEY:
			_launch_editor()

func _process(delta: float) -> void:
	if not _countdown_active:
		return
	_countdown_timer -= delta
	var secs = ceili(_countdown_timer)
	_countdown_number.text = str(max(secs, 0))
	if secs <= 2:
		_countdown_number.add_theme_color_override("font_color", UI_COUNTDOWN_COLOR)
	if _countdown_timer <= 0.0:
		_countdown_active = false
		_launch_game()

# =============================================
# CONFIG — load / save / apply
# =============================================

func _load_config() -> void:
	_cfg = ConfigFile.new()
	for section in DEFAULTS:
		_settings[section] = {}
		for key in DEFAULTS[section]:
			_settings[section][key] = DEFAULTS[section][key]
	if _cfg.load(CONFIG_PATH) == OK:
		for section in DEFAULTS:
			for key in DEFAULTS[section]:
				if _cfg.has_section_key(section, key):
					_settings[section][key] = _cfg.get_value(section, key)
	# what the engine started with (the project's default for this system, or user://override.cfg)
	for key in RESTART_SETTINGS:
		_settings["video"][key] = _engine_setting(key)

func _save_config() -> void:
	for section in _settings:
		for key in _settings[section]:
			_cfg.set_value(section, key, _settings[section][key])
	_cfg.save(CONFIG_PATH)

func _apply_settings() -> void:
	print("=== APPLY SETTINGS ===")
	print("resolution_index: ", _settings["video"]["resolution_index"])
	print("display_mode: ", _settings["video"]["display_mode"])
	if not is_instance_valid(get_tree()) or not is_instance_valid(get_tree().root):
		return
	var idx  = _settings["video"]["resolution_index"]
	var mode = _settings["video"]["display_mode"]
	var res: Vector2i
	var is_native = (idx == RESOLUTIONS.size() - 1)
	if is_native:
		res = DisplayServer.screen_get_size()
	else:
		res = RESOLUTIONS[idx]
	match mode:
		0:  # Windowed
			DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_WINDOWED)
			DisplayServer.window_set_size(res)
			DisplayServer.window_set_position(
				DisplayServer.screen_get_position() +
				(DisplayServer.screen_get_size() - res) / 2
			)
		1:  # Fullscreen (borderless)
			DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_FULLSCREEN)
		2:  # Exclusive Fullscreen
			DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_EXCLUSIVE_FULLSCREEN)
	get_tree().root.content_scale_size = res

	# 3D resolution: FSR upscales below 100 %, the 2D (menus, the game's own picture) stays sharp
	var scale: float = Global.RENDER_SCALES[clamp(_settings["video"]["render_scale"], 0, Global.RENDER_SCALES.size() - 1)]
	get_tree().root.scaling_3d_scale = scale
	get_tree().root.scaling_3d_mode = Viewport.SCALING_3D_MODE_FSR if scale < 1.0 else Viewport.SCALING_3D_MODE_BILINEAR

	# FPS limit
	var fps_idx = clamp(_settings["game"]["fps_limit"], 0, FPS_VALUES.size() - 1)
	Engine.max_fps = FPS_VALUES[fps_idx]

	# Bilinear filtering
	_apply_bilinear_filtering(_settings["video"].get("bilinear_filtering", 1))

func _read_controls_into_settings() -> void:
	_settings["video"]["resolution_index"] = _sel_resolution.selected
	_settings["video"]["display_mode"]     = _sel_display.selected

	_settings["video"]["bilinear_filtering"]  = _sel_bilinear_filter.selected
	_settings["video"]["separate_render_thread"] = _sel_render_thread.selected
	_settings["video"]["quality_preset"] = _sel_quality_preset.selected
	for i in QUALITY_KEYS.size():
		_settings["video"][QUALITY_KEYS[i]] = _sel_quality[i].selected

	_settings["audio"]["master_volume"] = int(_sl_master.value)
	_settings["audio"]["music_volume"]  = int(_sl_music.value)
	_settings["audio"]["sfx_volume"]    = int(_sl_sfx.value)
	_settings["audio"]["speech_volume"] = int(_sl_speech.value)
	_settings["audio"]["music_type"]      = _sel_music_type.selected
	_settings["audio"]["mc1_music"]       = _sel_mc1_music.selected
	_settings["audio"]["awe32_gm_intro"]  = _sel_awe32_gm_intro.selected
	_settings["audio"]["awe32_buffer_ms"] = int(_sp_awe32_buffer.value)

	_settings["input"]["invert_y"] = _sel_invert_y.selected

	_settings["game"]["level_mode"]   = _sel_level_mode.selected
	_settings["game"]["custom_level"] = Global.VALID_LEVELS[_sel_custom_level.selected]
	_settings["game"]["fps_limit"]    = _sel_fps.selected
	_settings["game"]["show_navigation"] = _sel_show_navigation.selected
	_settings["game"]["current_fog_index"] = _sel_fog_index.selected

	_settings["multiplayer"]["mode"]        = _sel_mp_mode.selected
	_settings["multiplayer"]["server_ip"]   = _le_mp_server_ip.text.strip_edges()
	_settings["multiplayer"]["server_port"] = int(_sp_mp_server_port.value)
	_settings["multiplayer"]["client_port"] = int(_sp_mp_client_port.value)
	_settings["multiplayer"]["debug"]       = _sel_mp_debug.selected
	_settings["multiplayer"]["record_file"] = _le_mp_record_file.text.strip_edges()

# =============================================
# RENDERING HELPERS
# =============================================

# Settings the engine reads only when it starts: they are written to user://override.cfg
# (application/config/project_settings_override) and the game restarts.
# - Godot's separate render thread is experimental (Godot warns it can crash; the game crashed on
#   Linux at random at the start, "corrupted size vs. prev_size" - maybe because of it, not proven;
#   and it lost the Vulkan device once in 17 replays on Windows). It is off by default (Godot's Safe
#   model) and the player can switch it on: 0.2-1.2 ms faster frames, 2.5-4 ms at p95/p99.
# - Per-vertex instead of per-pixel lighting, for weak graphics cards (the Low quality). It was saved
#   with ProjectSettings.save() into res://project.godot, which an exported game cannot write.
const OVERRIDE_PATH := "user://override.cfg"
const THREAD_MODEL_KEY := "driver/threads/thread_model"
const VERTEX_SHADING_KEY := "shading/overrides/force_vertex_shading"
const RESTART_SETTINGS := {
	"separate_render_thread": THREAD_MODEL_KEY,
	"force_vertex_shader":    VERTEX_SHADING_KEY,
}

# the engine's value for an Off/On setting of RESTART_SETTINGS
func _engine_value(key: String, on: bool) -> Variant:
	if key == "separate_render_thread":
		return 2 if on else 1
	return on

# the setting as the engine started with it: 0=Off 1=On
func _engine_setting(key: String) -> int:
	var value = ProjectSettings.get_setting_with_override("rendering/" + RESTART_SETTINGS[key])
	return 1 if value == _engine_value(key, true) else 0

# Returns true when the game restarts.
func _apply_restart_settings() -> bool:
	var cfg := ConfigFile.new()
	cfg.load(OVERRIDE_PATH) # a missing file is fine
	var changed := false
	for key in RESTART_SETTINGS:
		if _settings["video"][key] != _engine_setting(key):
			cfg.set_value("rendering", RESTART_SETTINGS[key], _engine_value(key, _settings["video"][key] == 1))
			changed = true
	if not changed:
		return false
	cfg.save(OVERRIDE_PATH)
	OS.create_process(OS.get_executable_path(), OS.get_cmdline_args())
	get_tree().quit()
	return true

# Toggles the default texture filtering used by 2D CanvasItems (UI controls,
# Sprite2D, etc. that don't have an explicit texture_filter override).
# 0 = Nearest (sharp/pixelated), 1 = Linear (bilinear, smooth).
# Note: this does NOT affect 3D mesh materials — those would need their own
# BaseMaterial3D.texture_filter override per material.
func _apply_bilinear_filtering(enabled_idx: int) -> void:	
	if not is_instance_valid(get_viewport()):
		return
	if enabled_idx == 1:
		Global.nearest_texture_filtering=0
		get_viewport().canvas_item_default_texture_filter = Viewport.DEFAULT_CANVAS_ITEM_TEXTURE_FILTER_LINEAR
	else:
		Global.nearest_texture_filtering=1
		get_viewport().canvas_item_default_texture_filter = Viewport.DEFAULT_CANVAS_ITEM_TEXTURE_FILTER_NEAREST

# =============================================
# COUNTDOWN UI
# =============================================

func _show_countdown() -> void:
	_countdown_canvas = CanvasLayer.new()
	_countdown_canvas.name = "CountdownCanvas"
	add_child(_countdown_canvas)

	var bg = ColorRect.new()
	bg.color = UI_LOADING_BG_COLOR
	bg.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_countdown_canvas.add_child(bg)

	_countdown_number = Label.new()
	_countdown_number.text = str(COUNTDOWN_SECONDS)
	_countdown_number.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_countdown_number.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	_countdown_number.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_countdown_number.offset_bottom = -120
	_countdown_number.add_theme_font_size_override("font_size", 96)
	_countdown_number.add_theme_color_override("font_color", UI_SPINNER_COLOR)
	_countdown_canvas.add_child(_countdown_number)

	_countdown_label = Label.new()
	_countdown_label.text = "LAUNCH_LOADING"
	_countdown_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_countdown_label.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	_countdown_label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_countdown_label.offset_top = 80
	_countdown_label.add_theme_font_size_override("font_size", 20)
	_countdown_label.add_theme_color_override("font_color", UI_TEXT_COLOR)
	_countdown_canvas.add_child(_countdown_label)

	var hint = Label.new()
	hint.text = "LAUNCH_HINT"
	hint.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	hint.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	hint.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	hint.offset_top = 140
	hint.add_theme_font_size_override("font_size", 14)
	hint.add_theme_color_override("font_color", Color(0.50, 0.50, 0.60))
	_countdown_canvas.add_child(hint)

	var buttons = HBoxContainer.new()
	buttons.add_theme_constant_override("separation", 16)
	buttons.grow_horizontal = Control.GROW_DIRECTION_BOTH # a longer translation widens it to both sides
	buttons.set_anchors_and_offsets_preset(Control.PRESET_CENTER_BOTTOM)
	buttons.offset_top    = -100
	buttons.offset_bottom = -50
	_countdown_canvas.add_child(buttons)
	for b in [["SETTINGS", _open_settings], ["CREDITS", _open_credits]]:
		var btn = Button.new()
		btn.text = b[0]
		btn.custom_minimum_size = Vector2(160, 0)
		_apply_button_theme(btn)
		btn.pressed.connect(b[1])
		buttons.add_child(btn)

	_reset_countdown(COUNTDOWN_SECONDS)

func _reset_countdown(secs: int) -> void:
	_countdown_timer = float(secs)
	if is_instance_valid(_countdown_number):
		_countdown_number.text = str(secs)
		_countdown_number.add_theme_color_override("font_color", UI_SPINNER_COLOR)
	_countdown_active = true

func _launch_game() -> void:
	MBDebug.log("launching the game")
	_countdown_active = false
	if _countdown_canvas:
		_countdown_canvas.queue_free()
	if is_instance_valid(_settings_canvas):
		_settings_canvas.queue_free()
	_apply_settings()
	var canvas = CanvasLayer.new()
	canvas.name = "GlobalLoadingCanvas"
	canvas.layer = 128
	get_tree().root.add_child(canvas)
	var bg = ColorRect.new()
	bg.color = Color(0, 0, 0, 1)
	bg.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	canvas.add_child(bg)
	var label = Label.new()
	label.text = "LAUNCH_STARTING_GAME"
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	label.offset_bottom = -80
	label.add_theme_font_size_override("font_size", 26)
	label.add_theme_color_override("font_color", UI_TEXT_COLOR)
	canvas.add_child(label)
	var spinner = Label.new()
	spinner.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	spinner.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	spinner.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	spinner.offset_top = 100
	spinner.add_theme_font_size_override("font_size", 48)
	spinner.add_theme_color_override("font_color", UI_SPINNER_COLOR)
	canvas.add_child(spinner)
	_animate_simple_spinner(spinner)
	SetGlobals()
	await get_tree().create_timer(0.1).timeout
	get_tree().change_scene_to_file("res://scenes/CodeGeneratedDemo.tscn")

func _launch_editor() -> void:
	_countdown_active = false
	if _countdown_canvas:
		_countdown_canvas.queue_free()
	if is_instance_valid(_settings_canvas):
		_settings_canvas.queue_free()
	_apply_settings()
	var canvas = CanvasLayer.new()
	canvas.name = "GlobalLoadingCanvas"
	canvas.layer = 128
	get_tree().root.add_child(canvas)
	var bg = ColorRect.new()
	bg.color = Color(0, 0, 0, 1)
	bg.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	canvas.add_child(bg)
	var label = Label.new()
	label.text = "LAUNCH_STARTING_EDITOR"
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	label.offset_bottom = -80
	label.add_theme_font_size_override("font_size", 26)
	label.add_theme_color_override("font_color", UI_TEXT_COLOR)
	canvas.add_child(label)
	var spinner = Label.new()
	spinner.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	spinner.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	spinner.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	spinner.offset_top = 100
	spinner.add_theme_font_size_override("font_size", 48)
	spinner.add_theme_color_override("font_color", UI_SPINNER_COLOR)
	canvas.add_child(spinner)
	_animate_simple_spinner(spinner)
	SetGlobals()
	await get_tree().create_timer(0.1).timeout
	get_tree().change_scene_to_file("res://scenes-editor/Editor.tscn")

func _reimport_scene() -> void:
	_countdown_active = false
	if _countdown_canvas:
		_countdown_canvas.queue_free()
	if is_instance_valid(_settings_canvas):
		_settings_canvas.queue_free()
	_apply_settings()
	var canvas = CanvasLayer.new()
	canvas.name = "GlobalLoadingCanvas"
	canvas.layer = 128
	get_tree().root.add_child(canvas)
	var bg = ColorRect.new()
	bg.color = Color(0, 0, 0, 1)
	bg.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	canvas.add_child(bg)
	var label = Label.new()
	label.text = "LAUNCH_LOADING_IMPORTER"
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	label.offset_bottom = -80
	label.add_theme_font_size_override("font_size", 26)
	label.add_theme_color_override("font_color", UI_TEXT_COLOR)
	canvas.add_child(label)
	var spinner = Label.new()
	spinner.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	spinner.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	spinner.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	spinner.offset_top = 100
	spinner.add_theme_font_size_override("font_size", 48)
	spinner.add_theme_color_override("font_color", UI_SPINNER_COLOR)
	canvas.add_child(spinner)
	_animate_simple_spinner(spinner)
	SetGlobals()
	await get_tree().create_timer(0.1).timeout
	get_tree().change_scene_to_file("res://scenes/Importer.tscn")

func SetGlobals() -> void:
	Global.master_volume   = _settings["audio"]["master_volume"] / 100.0
	Global.music_volume    = _settings["audio"]["music_volume"]  / 100.0
	Global.sounds_volume   = _settings["audio"]["sfx_volume"]    / 100.0
	Global.speech_volume   = _settings["audio"]["speech_volume"] / 100.0
	Global.music_type      = _settings["audio"]["music_type"]
	Global.himusic         = Global.music_type != Global.MUSIC_MIDI
	Global.mc1_music       = _settings["audio"]["mc1_music"] == 1
	Global.awe32_gm_intro  = _settings["audio"]["awe32_gm_intro"] == 1
	Global.awe32_buffer_ms = _settings["audio"]["awe32_buffer_ms"]
	Global.inverse_mouseY  = _settings["input"]["invert_y"]
	Global.level_mode      = _settings["game"]["level_mode"]
	Global.custom_level    = _settings["game"]["custom_level"]
	var fps_idx            = clamp(_settings["game"]["fps_limit"], 0, FPS_VALUES.size() - 1)
	Global.max_fps         = FPS_VALUES[fps_idx]
	Global.show_navigation = _settings["game"]["show_navigation"] == 1
	Global.force_vertex_shader = _settings["video"]["force_vertex_shader"] == 1
	Global.shadow_quality     = _settings["video"]["shadow_quality"]
	Global.reflections        = _settings["video"]["reflections"] == 1
	Global.current_fog_index = clamp(_settings["game"]["current_fog_index"], 0, Global.fog_presets.size() - 1)
	Global.mp_mode         = _settings["multiplayer"]["mode"]
	Global.mp_server_ip    = _settings["multiplayer"]["server_ip"]
	Global.mp_server_port  = _settings["multiplayer"]["server_port"]
	Global.mp_client_port  = _settings["multiplayer"]["client_port"]
	Global.mp_debug        = _settings["multiplayer"]["debug"] == 1
	Global.mp_record_file  = _settings["multiplayer"]["record_file"]

func _animate_simple_spinner(spinner: Label) -> void:
	var frames = ["⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"]
	var i = 0
	while is_instance_valid(spinner):
		spinner.text = frames[i % frames.size()]
		i += 1
		await get_tree().create_timer(0.1).timeout

# =============================================
# SETTINGS UI — open / close
# =============================================

func _open_settings() -> void:
	_countdown_active = false
	_settings_open    = true
	_language_before  = Global.text_language()

	_settings_canvas      = CanvasLayer.new()
	_settings_canvas.name = "SettingsCanvas"
	add_child(_settings_canvas)

	var backdrop = ColorRect.new()
	backdrop.color = Color(0, 0, 0, 0.85)
	backdrop.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_settings_canvas.add_child(backdrop)

	var panel = Panel.new()
	var screen = get_viewport().get_visible_rect().size
	var pw = min(screen.x * 0.85, 860.0)
	var ph = min(screen.y * 0.85, 620.0)
	panel.set_anchors_and_offsets_preset(Control.PRESET_CENTER)
	panel.offset_left   = -pw / 2.0
	panel.offset_right  =  pw / 2.0
	panel.offset_top    = -ph / 2.0
	panel.offset_bottom =  ph / 2.0
	var ps = StyleBoxFlat.new()
	ps.bg_color = UI_BG_COLOR
	ps.border_color = UI_BORDER_COLOR
	ps.set_border_width_all(UI_BORDER_WIDTH)
	ps.set_corner_radius_all(UI_CORNER_RADIUS)
	panel.add_theme_stylebox_override("panel", ps)
	_settings_canvas.add_child(panel)

	var root_vbox = VBoxContainer.new()
	root_vbox.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	panel.add_child(root_vbox)

	root_vbox.add_child(_make_header())

	var tabs = TabContainer.new()
	tabs.size_flags_vertical = Control.SIZE_EXPAND_FILL
	root_vbox.add_child(tabs)

	_build_video_tab(tabs)
	_build_audio_tab(tabs)
	_build_input_tab(tabs)
	_build_game_tab(tabs)
	_build_multiplayer_tab(tabs)

	root_vbox.add_child(_make_footer())

func _on_apply_pressed() -> void:
	_read_controls_into_settings()
	_save_config()
	# into the game's own setting; without game data yet the importer writes it after the import
	Global.write_game_language(_sel_language.selected + 1)
	if _apply_restart_settings():
		return
	_settings_open = false
	if is_instance_valid(_settings_canvas):
		_settings_canvas.queue_free()
	await get_tree().create_timer(0.08).timeout
	_launch_game()

func _on_cancel_pressed() -> void:
	_settings_open = false
	_settings_canvas.queue_free()
	Global.set_text_language(_language_before)
	_countdown_label.text = "LAUNCH_LOADING"
	_reset_countdown(COUNTDOWN_SECONDS)

# =============================================
# CREDITS
# =============================================

func _open_credits() -> void:
	if _settings_open or _credits_open:
		return
	_countdown_active = false
	_credits_open     = true

	_credits_canvas      = CanvasLayer.new()
	_credits_canvas.name = "CreditsCanvas"
	add_child(_credits_canvas)

	var backdrop = ColorRect.new()
	backdrop.color = Color(0, 0, 0, 0.85)
	backdrop.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_credits_canvas.add_child(backdrop)

	var panel = Panel.new()
	var screen = get_viewport().get_visible_rect().size
	var pw = min(screen.x * 0.85, 860.0)
	var ph = min(screen.y * 0.85, 620.0)
	panel.set_anchors_and_offsets_preset(Control.PRESET_CENTER)
	panel.offset_left   = -pw / 2.0
	panel.offset_right  =  pw / 2.0
	panel.offset_top    = -ph / 2.0
	panel.offset_bottom =  ph / 2.0
	var ps = StyleBoxFlat.new()
	ps.bg_color = UI_BG_COLOR
	ps.border_color = UI_BORDER_COLOR
	ps.set_border_width_all(UI_BORDER_WIDTH)
	ps.set_corner_radius_all(UI_CORNER_RADIUS)
	panel.add_theme_stylebox_override("panel", ps)
	_credits_canvas.add_child(panel)

	var root_vbox = VBoxContainer.new()
	root_vbox.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	panel.add_child(root_vbox)

	var header = _make_header()
	header.get_child(0).text = "CREDITS"
	root_vbox.add_child(header)

	var mg = MarginContainer.new()
	mg.size_flags_vertical = Control.SIZE_EXPAND_FILL
	for side in ["margin_top", "margin_bottom", "margin_left", "margin_right"]:
		mg.add_theme_constant_override(side, 16)
	root_vbox.add_child(mg)
	var rtl = RichTextLabel.new()
	rtl.bbcode_enabled = true
	rtl.auto_translate_mode = Node.AUTO_TRANSLATE_MODE_DISABLED # names and titles stay as they are
	rtl.add_theme_color_override("default_color", UI_TEXT_COLOR)
	rtl.add_theme_font_size_override("normal_font_size", 14)
	rtl.meta_clicked.connect(func(meta): OS.shell_open(str(meta)))
	rtl.text = _credits_bbcode()
	mg.add_child(rtl)

	var footer = HBoxContainer.new()
	footer.custom_minimum_size = Vector2(0, 52)
	footer.alignment = BoxContainer.ALIGNMENT_END
	var btn_close = Button.new()
	btn_close.text = "CLOSE"
	btn_close.custom_minimum_size = Vector2(110, 36)
	_apply_button_theme(btn_close)
	btn_close.pressed.connect(_close_credits)
	var margin = MarginContainer.new()
	margin.add_theme_constant_override("margin_right", 12)
	margin.add_child(btn_close)
	footer.add_child(margin)
	root_vbox.add_child(footer)

func _close_credits() -> void:
	_credits_open = false
	if is_instance_valid(_credits_canvas):
		_credits_canvas.queue_free()
	_reset_countdown(COUNTDOWN_SECONDS)

func _read_json(path: String) -> Variant:
	var text = FileAccess.get_file_as_string(path)
	return JSON.parse_string(text) if text != "" else null

# a name or title as plain text (some contain [ ])
func _bb(s: String) -> String:
	return s.replace("[", "\u0001").replace("]", "[rb]").replace("\u0001", "[lb]")

func _bb_link(text: String, url: String) -> String:
	return "[url=%s]%s[/url]" % [url, _bb(text)] if url != "" else _bb(text)

func _credits_bbcode() -> String:
	var title_color = "#" + UI_TITLE_COLOR.to_html(false)
	var dim = "[color=#9999b3]"
	var heading = func(key: String) -> String:
		return "\n[font_size=18][color=%s]%s[/color][/font_size]\n" % [title_color, tr(key)]
	var models = _read_json(MODELS_CREDITS_PATH)
	var other = _read_json(OTHER_CREDITS_PATH)
	if models == null:
		models = []
	if other == null:
		other = {}

	var s = "[center][font_size=24][color=%s]MagicBalls[/color][/font_size]\n%s[/center]\n" % [title_color, tr("CREDITS_INTRO")]

	s += heading.call("CREDITS_TEAM")
	for t in other.get("team", []):
		s += "[b]%s[/b]" % _bb_link(t["name"], t.get("url", ""))
		if t.get("role", "") != "":
			s += "  %s%s[/color]" % [dim, _bb(t["role"])]
		s += "\n"

	s += heading.call("CREDITS_MODELS")
	var category = ""
	var lost: Array = [] # models whose license is not known any more go to the thanks
	for m in models:
		if m.get("slug", "") == "unavailable":
			lost.append(m)
			continue
		if m["category"] != category:
			category = m["category"]
			s += "[color=%s]%s[/color]\n" % [title_color, _bb(category)]
		s += "  %s" % _bb_link(m["title"], m["url"])
		if m.get("author", "") != "":
			s += " – %s" % _bb_link(m["author"], m.get("author_url", ""))
		if m.get("license", "") != "":
			s += "  %s(%s)[/color]" % [dim, _bb_link(m["license"], m.get("license_url", ""))]
		s += "\n"

	s += heading.call("CREDITS_OTHER")
	for c in other.get("components", []):
		s += "%s – %s  %s(%s)[/color]\n" % [_bb_link(c["title"], c.get("url", "")), _bb(c.get("author", "")),
			dim, _bb_link(c["license"], c.get("license_url", ""))]

	s += heading.call("CREDITS_THANKS")
	for t in other.get("thanks", []):
		s += _bb_link(t["title"], t.get("url", ""))
		if t.get("author", "") != "":
			s += " – " + _bb(t["author"])
		s += "\n"
	for m in lost:
		s += "%s  %s(Sketchfab)[/color]\n" % [_bb_link(m["title"], m["url"]), dim]
	return s

# =============================================
# TAB BUILDERS
# =============================================

func _build_video_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("TAB_VIDEO", tc)

	# --- DISPLAY ---
	vbox.add_child(_section("SEC_DISPLAY"))
	_sel_resolution = _option(vbox, "SET_RESOLUTION", RESOLUTION_NAMES,
							  _settings["video"]["resolution_index"])
	_sel_display = _option(vbox, "SET_DISPLAY_MODE",
		["DISPLAY_WINDOWED", "DISPLAY_FULLSCREEN", "DISPLAY_EXCLUSIVE"],
		_settings["video"]["display_mode"])

	# --- QUALITY ---
	vbox.add_child(_section("SEC_QUALITY"))
	_sel_quality_preset = _option(vbox, "SET_QUALITY", ["Q_ULTRA", "Q_HIGH", "Q_MEDIUM", "Q_LOW", "Q_CUSTOM"],
		_settings["video"]["quality_preset"])
	_sel_quality = [
		_option(vbox, "SET_SHADOWS", ["OFF", "SH_LOW", "SH_MEDIUM", "SH_HIGH"], _settings["video"]["shadow_quality"]),
		_option(vbox, "SET_REFLECTIONS", ["OFF", "ON"], _settings["video"]["reflections"]),
		_option(vbox, "SET_RENDER_SCALE", ["100 %", "85 %", "70 %", "50 %"], _settings["video"]["render_scale"]),
		_option(vbox, "SET_FORCE_VERTEX", ["OFF", "ON"], _settings["video"]["force_vertex_shader"]),
	]
	vbox.add_child(_note("NOTE_QUALITY"))
	vbox.add_child(_note("NOTE_FORCE_VERTEX"))
	_sel_quality_preset.item_selected.connect(_on_quality_preset_selected)
	for sel in _sel_quality:
		sel.item_selected.connect(func(_idx): _update_quality_preset())
	_update_quality_preset() # the vertex shading comes from the engine and may not match the saved preset

	# --- RENDERING ---
	vbox.add_child(_section("SEC_RENDERING"))

	_sel_bilinear_filter = _option(vbox, "SET_BILINEAR", ["FILTER_NEAREST", "FILTER_LINEAR"],
		_settings["video"]["bilinear_filtering"])

	var note_filter = Label.new()
	note_filter.add_theme_font_size_override("font_size", 11)
	note_filter.add_theme_color_override("font_color", Color(0.60, 0.60, 0.70))
	note_filter.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	note_filter.text = "NOTE_BILINEAR"
	vbox.add_child(note_filter)

	_sel_render_thread = _option(vbox, "SET_RENDER_THREAD", ["OFF", "ON_EXPERIMENTAL"],
		_settings["video"]["separate_render_thread"])
	vbox.add_child(_warning_note("NOTE_RENDER_THREAD"))

func _build_audio_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("TAB_AUDIO", tc)
	vbox.add_child(_section("SEC_VOLUME"))
	_sl_master = _slider(vbox, "SET_MASTER_VOLUME", 0, 100, _settings["audio"]["master_volume"])
	_sl_music  = _slider(vbox, "SET_MUSIC",         0, 100, _settings["audio"]["music_volume"])
	_sl_sfx    = _slider(vbox, "SET_SOUNDS",        0, 100, _settings["audio"]["sfx_volume"])
	_sl_speech = _slider(vbox, "SET_SPEECH",        0, 100, _settings["audio"]["speech_volume"])

	vbox.add_child(_section("SEC_MUSIC"))
	_sel_music_type = _option(vbox, "SET_MUSIC_TYPE",
		["MUSIC_OGG", "MUSIC_MIDI", "MUSIC_AWE32"],
		_settings["audio"]["music_type"])
	_sel_mc1_music = _option(vbox, "SET_MC1_MUSIC", ["OFF", "ON"],
		_settings["audio"]["mc1_music"])
	_sel_awe32_gm_intro = _option(vbox, "SET_GM_INTRO", ["OFF", "ON"],
		_settings["audio"]["awe32_gm_intro"])
	_sp_awe32_buffer = _spin(vbox, "SET_AWE32_BUFFER", 100, 2000, _settings["audio"]["awe32_buffer_ms"])

	var note_music = Label.new()
	note_music.add_theme_font_size_override("font_size", 11)
	note_music.add_theme_color_override("font_color", Color(0.60, 0.60, 0.70))
	note_music.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	vbox.add_child(note_music)

	var _update_music = func(idx: int) -> void:
		var awe32 = (idx == Global.MUSIC_AWE32)
		_sel_mc1_music.get_parent().visible      = awe32
		_sel_awe32_gm_intro.get_parent().visible = awe32
		_sp_awe32_buffer.get_parent().visible    = awe32
		match idx:
			Global.MUSIC_OGG:
				note_music.text = "NOTE_MUSIC_OGG"
			Global.MUSIC_MIDI:
				note_music.text = "NOTE_MUSIC_MIDI"
			_:
				note_music.text = "NOTE_MUSIC_AWE32"
	_sel_music_type.item_selected.connect(_update_music)
	_update_music.call(_settings["audio"]["music_type"])

func _build_input_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("TAB_INPUT", tc)
	vbox.add_child(_section("SEC_MOUSE_CAMERA"))
	_sel_invert_y = _option(vbox, "SET_INVERT_Y", ["NO", "YES"],
		_settings["input"]["invert_y"])

const FPS_VALUES = [20, 24, 30, 40, 50]
const FPS_NAMES  = ["20", "24", "30", "40", "50"]

func _build_game_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("TAB_GAME", tc)
	vbox.add_child(_section("SEC_LANGUAGE"))
	# the names in their own languages, not translated; the choice shows at once
	_sel_language = _option(vbox, "SET_LANGUAGE", Global.LANG_NAMES, max(Global.text_language() - 1, 0))
	_sel_language.auto_translate_mode = Node.AUTO_TRANSLATE_MODE_DISABLED
	_sel_language.item_selected.connect(func(idx): Global.set_text_language(idx + 1))
	vbox.add_child(_note("NOTE_LANGUAGE"))

	vbox.add_child(_section("SEC_LEVEL"))
	_sel_level_mode = _option(vbox, "SET_GAME_MODE",
		["MODE_STANDARD", "MODE_CUSTOM"],
		_settings["game"]["level_mode"])
	var level_names: Array = []
	for lvl in Global.VALID_LEVELS:
		level_names.append(str(lvl))
	var saved_opt_idx = max(Global.VALID_LEVELS.find(_settings["game"]["custom_level"]), 0)
	_sel_custom_level = _option(vbox, "SET_LEVEL_NUMBER", level_names, saved_opt_idx)
	_sel_level_mode.item_selected.connect(_on_level_mode_changed)
	_update_custom_level_visibility()

	_sel_fps = _option(vbox, "SET_FPS_LIMIT", FPS_NAMES, _settings["game"]["fps_limit"])

	vbox.add_child(_section("SEC_NAVIGATION"))
	_sel_show_navigation = _option(vbox, "SET_SHOW_NAVIGATION", ["OFF", "ON"], _settings["game"]["show_navigation"])

	vbox.add_child(_section("SEC_FOG"))
	var fog_names: Array = []
	for i in range(Global.fog_presets.size()):
		fog_names.append(str(i))
	var saved_fog_idx = clamp(_settings["game"]["current_fog_index"], 0, Global.fog_presets.size() - 1)
	_sel_fog_index = _option(vbox, "SET_FOG_PRESET", fog_names, saved_fog_idx)

	var fps_note = Label.new()
	fps_note.add_theme_color_override("font_color", Color(1.0, 0.75, 0.3))
	fps_note.add_theme_font_size_override("font_size", 12)
	fps_note.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	vbox.add_child(fps_note)

	var _update_fps_note = func(idx: int) -> void:
		if FPS_VALUES[idx] >= 30:
			fps_note.text = "NOTE_FPS"
		else:
			fps_note.text = ""
	_sel_fps.item_selected.connect(_update_fps_note)
	_update_fps_note.call(_settings["game"]["fps_limit"])

func _build_multiplayer_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("TAB_MULTIPLAYER", tc)
	vbox.add_child(_section("SEC_NETWORK_GAME"))
	_sel_mp_mode = _option(vbox, "SET_MP_MODE", ["MP_OFF", "MP_HOST", "MP_JOIN"],
		_settings["multiplayer"]["mode"])
	_le_mp_server_ip = _line_edit(vbox, "SET_SERVER_IP", _settings["multiplayer"]["server_ip"],
		"PH_SERVER_IP")
	_sp_mp_server_port = _spin(vbox, "SET_SERVER_PORT", 0, 65535, _settings["multiplayer"]["server_port"])
	_sp_mp_client_port = _spin(vbox, "SET_CLIENT_PORT", 0, 65535, _settings["multiplayer"]["client_port"])
	_le_mp_record_file = _line_edit(vbox, "SET_RECORD_TO", _settings["multiplayer"]["record_file"],
		"PH_RECORD")
	_sel_mp_debug = _option(vbox, "SET_DEBUG_NETWORK", ["OFF", "ON"], _settings["multiplayer"]["debug"])

	vbox.add_child(_note("NOTE_MULTIPLAYER"))

	_sel_mp_mode.item_selected.connect(func(_idx): _update_multiplayer_visibility())
	_update_multiplayer_visibility()

func _update_multiplayer_visibility() -> void:
	var mode = _sel_mp_mode.selected
	_le_mp_server_ip.get_parent().visible   = (mode == 2)
	_sp_mp_server_port.get_parent().visible = (mode != 0)
	_sp_mp_client_port.get_parent().visible = (mode == 2)
	_le_mp_record_file.get_parent().visible = (mode == 1)
	_sel_mp_debug.get_parent().visible      = (mode != 0)

# a preset sets the four quality settings
func _on_quality_preset_selected(idx: int) -> void:
	if idx == QUALITY_CUSTOM:
		return
	for i in QUALITY_KEYS.size():
		_sel_quality[i].select(QUALITY_PRESETS[idx][i])

# a changed quality setting: the preset it matches, else Custom
func _update_quality_preset() -> void:
	var values = _sel_quality.map(func(sel): return sel.selected)
	var idx = QUALITY_PRESETS.find(values)
	_sel_quality_preset.select(idx if idx >= 0 else QUALITY_CUSTOM)

func _on_level_mode_changed(_idx: int) -> void:
	_update_custom_level_visibility()

func _update_custom_level_visibility() -> void:
	_sel_custom_level.get_parent().visible = (_sel_level_mode.selected == 1)

# =============================================
# UI HELPERS
# =============================================

func _make_header() -> PanelContainer:
	var pc = PanelContainer.new()
	var sty = StyleBoxFlat.new()
	sty.bg_color = UI_TREE_BG_COLOR
	sty.border_color = UI_TREE_BORDER_COLOR
	sty.border_width_bottom = 1
	pc.add_theme_stylebox_override("panel", sty)
	var lbl = Label.new()
	lbl.text = "SETTINGS" # a key of translations/texts.csv, like the texts below
	lbl.add_theme_color_override("font_color", UI_TITLE_COLOR)
	lbl.add_theme_font_size_override("font_size", UI_TITLE_FONT_SIZE)
	lbl.add_theme_constant_override("margin_left", 16)
	pc.add_child(lbl)
	return pc

func _make_footer() -> HBoxContainer:
	var hbox = HBoxContainer.new()
	hbox.custom_minimum_size = Vector2(0, 52)
	hbox.alignment = BoxContainer.ALIGNMENT_END
	hbox.add_theme_constant_override("separation", 10)
	var btn_cancel = Button.new()
	btn_cancel.text = "CANCEL"
	btn_cancel.custom_minimum_size = Vector2(110, 36)
	_apply_button_theme(btn_cancel)
	btn_cancel.pressed.connect(_on_cancel_pressed)
	hbox.add_child(btn_cancel)
	var btn_apply = Button.new()
	btn_apply.text = "APPLY_CONTINUE"
	btn_apply.custom_minimum_size = Vector2(180, 36)
	_apply_button_theme(btn_apply)
	btn_apply.pressed.connect(_on_apply_pressed)
	var margin = MarginContainer.new()
	margin.add_theme_constant_override("margin_right", 12)
	margin.add_child(btn_apply)
	hbox.add_child(margin)
	return hbox

func _make_tab(tab_name: String, tc: TabContainer) -> VBoxContainer:
	var scroll = ScrollContainer.new()
	scroll.name = tab_name
	scroll.horizontal_scroll_mode = ScrollContainer.SCROLL_MODE_DISABLED
	scroll.size_flags_vertical = Control.SIZE_EXPAND_FILL
	tc.add_child(scroll)
	var mg = MarginContainer.new()
	mg.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	mg.add_theme_constant_override("margin_top",    16)
	mg.add_theme_constant_override("margin_bottom", 16)
	mg.add_theme_constant_override("margin_left",   20)
	mg.add_theme_constant_override("margin_right",  20)
	scroll.add_child(mg)
	var vbox = VBoxContainer.new()
	vbox.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	vbox.add_theme_constant_override("separation", 6)
	mg.add_child(vbox)
	return vbox

# a small grey explanation under a setting
func _note(text: String) -> Label:
	var lbl = Label.new()
	lbl.add_theme_font_size_override("font_size", 11)
	lbl.add_theme_color_override("font_color", Color(0.60, 0.60, 0.70))
	lbl.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	lbl.text = text
	return lbl

func _section(text: String) -> Label:
	var lbl = Label.new()
	lbl.text = text
	lbl.add_theme_color_override("font_color", UI_TITLE_COLOR)
	lbl.add_theme_font_size_override("font_size", 13)
	return lbl

# a note with a yellow warning triangle in front of it (drawn - the default font may not have ⚠)
func _warning_note(text: String) -> HBoxContainer:
	var row = HBoxContainer.new()
	row.add_theme_constant_override("separation", 8)
	var icon = Control.new()
	icon.custom_minimum_size = Vector2(20, 18)
	icon.size_flags_vertical = Control.SIZE_SHRINK_BEGIN
	icon.draw.connect(func():
		icon.draw_colored_polygon(PackedVector2Array([Vector2(10, 0), Vector2(20, 18), Vector2(0, 18)]), Color(1.0, 0.8, 0.1))
		icon.draw_rect(Rect2(9, 5, 2, 7), Color.BLACK)
		icon.draw_rect(Rect2(9, 14, 2, 2), Color.BLACK))
	row.add_child(icon)
	var lbl = Label.new()
	lbl.add_theme_font_size_override("font_size", 11)
	lbl.add_theme_color_override("font_color", Color(1.0, 0.85, 0.45))
	lbl.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	lbl.text = text
	row.add_child(lbl)
	return row

func _option(parent: VBoxContainer, label_text: String,
		items: Array, selected_idx: int) -> OptionButton:
	var row = HBoxContainer.new()
	row.custom_minimum_size = Vector2(0, 36)
	parent.add_child(row)
	var lbl = Label.new()
	lbl.text = label_text
	lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	lbl.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	lbl.add_theme_color_override("font_color", UI_TEXT_COLOR)
	lbl.add_theme_font_size_override("font_size", 14)
	row.add_child(lbl)
	var opt = OptionButton.new()
	opt.custom_minimum_size = Vector2(180, 32)
	opt.add_theme_color_override("font_color", UI_TEXT_COLOR)
	for item in items:
		opt.add_item(item)
	opt.select(selected_idx)
	row.add_child(opt)
	return opt

func _line_edit(parent: VBoxContainer, label_text: String,
		value: String, placeholder: String = "") -> LineEdit:
	var row = HBoxContainer.new()
	row.custom_minimum_size = Vector2(0, 36)
	parent.add_child(row)
	var lbl = Label.new()
	lbl.text = label_text
	lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	lbl.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	lbl.add_theme_color_override("font_color", UI_TEXT_COLOR)
	lbl.add_theme_font_size_override("font_size", 14)
	row.add_child(lbl)
	var le = LineEdit.new()
	le.text = value
	le.placeholder_text = placeholder
	le.custom_minimum_size = Vector2(240, 32)
	le.add_theme_color_override("font_color", UI_TEXT_COLOR)
	row.add_child(le)
	return le

func _spin(parent: VBoxContainer, label_text: String,
		min_val: int, max_val: int, value: int) -> SpinBox:
	var row = HBoxContainer.new()
	row.custom_minimum_size = Vector2(0, 36)
	parent.add_child(row)
	var lbl = Label.new()
	lbl.text = label_text
	lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	lbl.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	lbl.add_theme_color_override("font_color", UI_TEXT_COLOR)
	lbl.add_theme_font_size_override("font_size", 14)
	row.add_child(lbl)
	var sp = SpinBox.new()
	sp.min_value = min_val
	sp.max_value = max_val
	sp.step      = 1
	sp.value     = value
	sp.custom_minimum_size = Vector2(180, 32)
	row.add_child(sp)
	return sp

func _slider(parent: VBoxContainer, label_text: String,
		min_val: float, max_val: float, value: float) -> HSlider:
	var row = HBoxContainer.new()
	row.custom_minimum_size = Vector2(0, 36)
	parent.add_child(row)
	var lbl = Label.new()
	lbl.text = label_text
	lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	lbl.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	lbl.add_theme_color_override("font_color", UI_TEXT_COLOR)
	lbl.add_theme_font_size_override("font_size", 14)
	row.add_child(lbl)
	var sl = HSlider.new()
	sl.min_value = min_val
	sl.max_value = max_val
	sl.step      = 1
	sl.value     = value
	sl.custom_minimum_size = Vector2(160, 32)
	row.add_child(sl)
	var val_lbl = Label.new()
	val_lbl.text = str(int(value))
	val_lbl.custom_minimum_size = Vector2(36, 0)
	val_lbl.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
	val_lbl.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	val_lbl.add_theme_color_override("font_color", UI_TEXT_SELECT_COLOR)
	val_lbl.add_theme_font_size_override("font_size", 14)
	row.add_child(val_lbl)
	sl.value_changed.connect(func(v): val_lbl.text = str(int(v)))
	return sl

# =============================================
# THEME HELPERS
# =============================================

func _apply_button_theme(btn: Button) -> void:
	btn.add_theme_stylebox_override("normal",  _make_btn_style(UI_BTN_NORMAL_COLOR, UI_BTN_BORDER_COLOR))
	btn.add_theme_stylebox_override("hover",   _make_btn_style(UI_BTN_HOVER_COLOR,  UI_BTN_HOVER_BORDER))
	btn.add_theme_stylebox_override("pressed", _make_btn_style(UI_BTN_PRESS_COLOR,  UI_BTN_BORDER_COLOR))
	btn.add_theme_color_override("font_color", UI_BTN_TEXT_COLOR)

func _make_btn_style(bg: Color, border: Color) -> StyleBoxFlat:
	var s = StyleBoxFlat.new()
	s.bg_color     = bg
	s.border_color = border
	s.set_border_width_all(2)
	s.set_corner_radius_all(4)
	s.content_margin_left  = 12 # a longer translation does not touch the border
	s.content_margin_right = 12
	return s
