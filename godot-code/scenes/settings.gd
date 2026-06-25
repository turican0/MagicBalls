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
		"hdr_enabled":      0,   # 0=Off 1=On (requires display/window/hdr/request_hdr_output=true in project.godot)
		"hdr_ref_luminance": 0,  # Brightness / Paperwhite in nits; 0 = Auto (-1 in API); Windows only
		"hdr_max_luminance": 0,  # Max luminance in nits;   0 = Auto (-1 in API); Windows + macOS
		"force_vertex_shader": 0, # 0=Off 1=On (forces per-vertex shading instead of per-pixel, for low-end GPUs)
	},
	"audio": {
		"master_volume":    100,
		"music_volume":     100,
		"sfx_volume":       100,
		"speech_volume":    100,
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

const RESOLUTION_NAMES = [
	"640 × 480",
	"800 × 600",
	"1280 × 720",
	"1920 × 1080",
	"2560 × 1440",
	"3840 × 2160",
	"Native Resolution",
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

# Control refs (filled during UI build)
var _sel_resolution:    OptionButton
var _sel_display:       OptionButton
var _sel_vsync:         OptionButton
var _sel_texture:       OptionButton
var _sl_view_dist:      HSlider
var _sel_fps:           OptionButton
var _sel_show_navigation: OptionButton

# HDR controls
var _sel_hdr:           OptionButton
var _lbl_hdr_warn:      Label
var _sl_hdr_ref:        HSlider  # Jas / Paperwhite (pouze Windows)
var _sl_hdr_max:        HSlider  # Max jas (Windows + macOS)

# Rendering controls
var _sel_force_vertex:  OptionButton

var _sl_master:         HSlider
var _sl_music:          HSlider
var _sl_sfx:            HSlider
var _sl_speech:         HSlider

var _sl_sensitivity:    HSlider
var _sel_invert_y:      OptionButton
var _sel_gamepad:       OptionButton

var _sel_language:      OptionButton
var _sel_difficulty:    OptionButton
var _sel_autosave:      OptionButton

var _sel_level_mode:    OptionButton
var _sel_custom_level:  OptionButton

# =============================================
# READY
# =============================================

func _ready() -> void:
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
		if event.keycode == KEY_ESCAPE:
			_countdown_active = false
			_launch_game()
		elif event.keycode == SETTINGS_KEY:
			_open_settings()
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

	# FPS limit
	var fps_idx = clamp(_settings["game"]["fps_limit"], 0, FPS_VALUES.size() - 1)
	Engine.max_fps = FPS_VALUES[fps_idx]

	# HDR (Godot 4.7+)
	_apply_hdr(
		_settings["video"].get("hdr_enabled",      0),
		_settings["video"].get("hdr_ref_luminance", 0),
		_settings["video"].get("hdr_max_luminance", 0)
	)

	# Force vertex shading
	_apply_force_vertex_shader(_settings["video"].get("force_vertex_shader", 0))

func _read_controls_into_settings() -> void:
	_settings["video"]["resolution_index"] = _sel_resolution.selected
	_settings["video"]["display_mode"]     = _sel_display.selected

	# HDR
	_settings["video"]["hdr_enabled"] = _sel_hdr.selected if not _sel_hdr.disabled else 0
	if _hdr_ref_lum_supported() and is_instance_valid(_sl_hdr_ref):
		_settings["video"]["hdr_ref_luminance"] = int(_sl_hdr_ref.value)
	if _hdr_max_lum_supported() and is_instance_valid(_sl_hdr_max):
		_settings["video"]["hdr_max_luminance"] = int(_sl_hdr_max.value)

	# Force vertex shading
	_settings["video"]["force_vertex_shader"] = _sel_force_vertex.selected

	_settings["audio"]["master_volume"] = int(_sl_master.value)
	_settings["audio"]["music_volume"]  = int(_sl_music.value)
	_settings["audio"]["sfx_volume"]    = int(_sl_sfx.value)
	_settings["audio"]["speech_volume"] = int(_sl_speech.value)

	_settings["input"]["invert_y"] = _sel_invert_y.selected

	_settings["game"]["level_mode"]   = _sel_level_mode.selected
	_settings["game"]["custom_level"] = Global.VALID_LEVELS[_sel_custom_level.selected]
	_settings["game"]["fps_limit"]    = _sel_fps.selected
	_settings["game"]["show_navigation"] = _sel_show_navigation.selected

# =============================================
# HDR HELPERS  (Godot 4.7+)
# Zdroj API: godot-demo-projects/misc/hdr_output
# =============================================

# Returns true if the current window/display supports HDR output.
# Internally calls DisplayServer.window_is_hdr_output_supported() — available since 4.7.
func _is_hdr_supported() -> bool:
	if not DisplayServer.has_method("window_is_hdr_output_supported"):
		return false
	return DisplayServer.window_is_hdr_output_supported(get_window().get_window_id())

# Manual brightness (paperwhite) override is supported on Windows only.
func _hdr_ref_lum_supported() -> bool:
	return DisplayServer.get_name() == &"Windows"

# Manual max luminance override is supported on Windows and macOS.
func _hdr_max_lum_supported() -> bool:
	var ds := DisplayServer.get_name()
	return ds == &"Windows" or ds == &"macOS" \
		or (ds == &"embedded" and OS.get_name() == &"macOS")

# Applies HDR settings to the main window.
# enabled_idx: 0=Off 1=On
# ref_lum / max_lum: value in nits; 0 = Auto (passes -1 to the API)
func _apply_hdr(enabled_idx: int, ref_lum: int, max_lum: int) -> void:
	# window.hdr_output_requested is a runtime toggle (Window property, not DisplayServer).
	# Requires the following in project.godot:
	#   display/window/hdr/request_hdr_output = true
	get_window().hdr_output_requested = (enabled_idx == 1)

	if enabled_idx == 1:
		var window_id := get_window().get_window_id()
		if _hdr_ref_lum_supported():
			DisplayServer.window_set_hdr_output_reference_luminance(
				float(ref_lum) if ref_lum > 0 else -1.0, window_id)
		if _hdr_max_lum_supported():
			DisplayServer.window_set_hdr_output_max_luminance(
				float(max_lum) if max_lum > 0 else -1.0, window_id)

# =============================================
# RENDERING HELPERS
# =============================================

# Forces per-vertex shading instead of per-pixel shading.
# Godot 4 has no built-in global ProjectSettings switch equivalent to Godot 3's
# "rendering/quality/shading/force_vertex_shading", so this stores the flag in
# Global for the rest of the game (materials/shaders) to read and react to.
func _apply_force_vertex_shader(enabled_idx: int) -> void:
	var enabled := (enabled_idx == 1)
	if ProjectSettings.get_setting("rendering/shading/overrides/force_vertex_shading", false) != enabled:
		ProjectSettings.set_setting("rendering/shading/overrides/force_vertex_shading", enabled)
		ProjectSettings.save()

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
	_countdown_label.text = "Loading..."
	_countdown_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_countdown_label.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	_countdown_label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_countdown_label.offset_top = 80
	_countdown_label.add_theme_font_size_override("font_size", 20)
	_countdown_label.add_theme_color_override("font_color", UI_TEXT_COLOR)
	_countdown_canvas.add_child(_countdown_label)

	var hint = Label.new()
	hint.text = "Press  [Esc]  to START NOW  |  Press  [R]  for Reimport data |  Press  [S]  for Settings |  Press  [E]  for Editor"
	hint.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	hint.vertical_alignment   = VERTICAL_ALIGNMENT_CENTER
	hint.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	hint.offset_top = 140
	hint.add_theme_font_size_override("font_size", 14)
	hint.add_theme_color_override("font_color", Color(0.50, 0.50, 0.60))
	_countdown_canvas.add_child(hint)

	var btn = Button.new()
	btn.text = "SETTINGS"
	btn.set_anchors_and_offsets_preset(Control.PRESET_CENTER_BOTTOM)
	btn.offset_top    = -100
	btn.offset_bottom = -50
	btn.offset_left   = -80
	btn.offset_right  = 80
	_apply_button_theme(btn)
	btn.pressed.connect(_open_settings)
	_countdown_canvas.add_child(btn)

	_reset_countdown(COUNTDOWN_SECONDS)

func _reset_countdown(secs: int) -> void:
	_countdown_timer = float(secs)
	if is_instance_valid(_countdown_number):
		_countdown_number.text = str(secs)
		_countdown_number.add_theme_color_override("font_color", UI_SPINNER_COLOR)
	_countdown_active = true

func _launch_game() -> void:
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
	label.text = "Starting game MagicBalls"
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
	label.text = "Starting game Editor"
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
	label.text = "Loading Importer"
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
	Global.inverse_mouseY  = _settings["input"]["invert_y"]
	Global.level_mode      = _settings["game"]["level_mode"]
	Global.custom_level    = _settings["game"]["custom_level"]
	var fps_idx            = clamp(_settings["game"]["fps_limit"], 0, FPS_VALUES.size() - 1)
	Global.max_fps         = FPS_VALUES[fps_idx]
	Global.show_navigation = _settings["game"]["show_navigation"] == 1
	Global.force_vertex_shader = _settings["video"]["force_vertex_shader"] == 1

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

	root_vbox.add_child(_make_footer())

func _on_apply_pressed() -> void:
	_read_controls_into_settings()
	_save_config()
	_settings_open = false
	if is_instance_valid(_settings_canvas):
		_settings_canvas.queue_free()
	await get_tree().create_timer(0.08).timeout
	_launch_game()

func _on_cancel_pressed() -> void:
	_settings_open = false
	_settings_canvas.queue_free()
	_countdown_label.text = "Loading..."
	_reset_countdown(COUNTDOWN_SECONDS)

# =============================================
# TAB BUILDERS
# =============================================

func _build_video_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("VIDEO", tc)

	# --- DISPLAY ---
	vbox.add_child(_section("DISPLAY"))
	_sel_resolution = _option(vbox, "Resolution", RESOLUTION_NAMES,
							  _settings["video"]["resolution_index"])
	_sel_display = _option(vbox, "Display Mode",
		["Windowed", "Fullscreen", "Exclusive Fullscreen"],
		_settings["video"]["display_mode"])

	# --- HDR (Godot 4.7+) ---
	vbox.add_child(_section("HDR"))

	var hdr_ok := _is_hdr_supported()

	# On/Off toggle
	_sel_hdr = _option(vbox, "HDR Output", ["Off", "On"],
		_settings["video"]["hdr_enabled"] if hdr_ok else 0)
	if not hdr_ok:
		_sel_hdr.disabled = true
		_sel_hdr.select(0)

	# Status label below the toggle
	_lbl_hdr_warn = Label.new()
	_lbl_hdr_warn.add_theme_font_size_override("font_size", 12)
	_lbl_hdr_warn.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	if hdr_ok:
		_lbl_hdr_warn.text = "✓ HDR output is supported on this display."
		_lbl_hdr_warn.add_theme_color_override("font_color", Color(0.40, 1.00, 0.50))
	else:
		_lbl_hdr_warn.text = "⚠ HDR is not available (display, GPU, or platform does not support it). " \
			+ "On Linux, Wayland is required. The Compatibility renderer does not support HDR."
		_lbl_hdr_warn.add_theme_color_override("font_color", Color(1.00, 0.40, 0.40))
	vbox.add_child(_lbl_hdr_warn)

	# Brightness / Paperwhite — Windows only
	# Value in nits; 0 = Auto (passed to the API as -1).
	var saved_ref: int = _settings["video"]["hdr_ref_luminance"]
	_sl_hdr_ref = _slider(vbox, "Brightness / Paperwhite (nits)", 80, 500,
		saved_ref if saved_ref > 0 else 200)
	# Hide the parent HBoxContainer if the platform does not support this
	_sl_hdr_ref.get_parent().visible = hdr_ok and _hdr_ref_lum_supported()

	# Max luminance — Windows + macOS
	var saved_max: int = _settings["video"]["hdr_max_luminance"]
	_sl_hdr_max = _slider(vbox, "Max Luminance (nits)", 200, 2000,
		saved_max if saved_max > 0 else 1000)
	_sl_hdr_max.get_parent().visible = hdr_ok and _hdr_max_lum_supported()

	# Note about project.godot
	var note_hdr = Label.new()
	note_hdr.add_theme_font_size_override("font_size", 11)
	note_hdr.add_theme_color_override("font_color", Color(0.60, 0.60, 0.70))
	note_hdr.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	note_hdr.text = "HDR requires in project.godot: display/window/hdr/request_hdr_output = true"
	vbox.add_child(note_hdr)

	# --- RENDERING ---
	vbox.add_child(_section("RENDERING"))
	_sel_force_vertex = _option(vbox, "Force Vertex Shader", ["Off", "On"],
		_settings["video"]["force_vertex_shader"])

	var note_vertex = Label.new()
	note_vertex.add_theme_font_size_override("font_size", 11)
	note_vertex.add_theme_color_override("font_color", Color(0.60, 0.60, 0.70))
	note_vertex.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	note_vertex.text = "Forces per-vertex shading instead of per-pixel — improves performance on low-end GPUs at the cost of lighting quality."
	vbox.add_child(note_vertex)

func _build_audio_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("AUDIO", tc)
	vbox.add_child(_section("VOLUME"))
	_sl_master = _slider(vbox, "Master Volume", 0, 100, _settings["audio"]["master_volume"])
	_sl_music  = _slider(vbox, "Music",         0, 100, _settings["audio"]["music_volume"])
	_sl_sfx    = _slider(vbox, "Sounds",        0, 100, _settings["audio"]["sfx_volume"])
	_sl_speech = _slider(vbox, "Speech",        0, 100, _settings["audio"]["speech_volume"])

func _build_input_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("INPUT", tc)
	vbox.add_child(_section("MOUSE & CAMERA"))
	_sel_invert_y = _option(vbox, "Invert Y Axis", ["No", "Yes"],
		_settings["input"]["invert_y"])

const FPS_VALUES = [20, 24, 30, 40, 50]
const FPS_NAMES  = ["20", "24", "30", "40", "50"]

func _build_game_tab(tc: TabContainer) -> void:
	var vbox = _make_tab("GAME", tc)
	vbox.add_child(_section("LEVEL"))
	_sel_level_mode = _option(vbox, "Game Mode",
		["Standard Game", "Custom Level"],
		_settings["game"]["level_mode"])
	var level_names: Array = []
	for lvl in Global.VALID_LEVELS:
		level_names.append(str(lvl))
	var saved_opt_idx = max(Global.VALID_LEVELS.find(_settings["game"]["custom_level"]), 0)
	_sel_custom_level = _option(vbox, "Level Number", level_names, saved_opt_idx)
	_sel_level_mode.item_selected.connect(_on_level_mode_changed)
	_update_custom_level_visibility()

	_sel_fps = _option(vbox, "FPS Limit (game speed)", FPS_NAMES, _settings["game"]["fps_limit"])

	vbox.add_child(_section("NAVIGATION"))
	_sel_show_navigation = _option(vbox, "Show Navigation", ["Off", "On"], _settings["game"]["show_navigation"])

	var fps_note = Label.new()
	fps_note.add_theme_color_override("font_color", Color(1.0, 0.75, 0.3))
	fps_note.add_theme_font_size_override("font_size", 12)
	fps_note.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	vbox.add_child(fps_note)

	var _update_fps_note = func(idx: int) -> void:
		if FPS_VALUES[idx] >= 30:
			fps_note.text = "⚠ Warning: at higher FPS the game becomes harder to control — " \
				+ "only for hardcore players (best experience is at 24 FPS)."
		else:
			fps_note.text = ""
	_sel_fps.item_selected.connect(_update_fps_note)
	_update_fps_note.call(_settings["game"]["fps_limit"])

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
	lbl.text = "SETTINGS"
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
	btn_apply.text = "APPLY & CONTINUE"
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

func _section(text: String) -> Label:
	var lbl = Label.new()
	lbl.text = text
	lbl.add_theme_color_override("font_color", UI_TITLE_COLOR)
	lbl.add_theme_font_size_override("font_size", 13)
	return lbl

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
	return s
