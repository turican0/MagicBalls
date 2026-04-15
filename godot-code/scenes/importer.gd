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

var _thread: Thread
var Main_DecodeLevel

# Picker State
var _picker_canvas: CanvasLayer
var _picker_tree: Tree
var _picker_path_label: Label
var _picker_current_path: String = ""
var _picker_selected_path: String = ""

# Control State
var _stop_spinner: bool = false

func _ready() -> void:
	Global.canNotification = false
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	Main_DecodeLevel = get_node("DecodeLevel")
	
	await get_tree().process_frame
	
	if !check_existing_data():
		_show_welcome_dialog()
	else:
		_go_to_main_menu()

func _go_to_main_menu():
	Global.last_scene_path = "res://scenes/CodeGeneratedDemo.tscn"
	get_tree().change_scene_to_file("res://scenes/CodeGeneratedDemo.tscn")

func check_existing_data() -> bool:
	var music_path = Global.convertdata + "musicsX/"
	return DirAccess.dir_exists_absolute(music_path)

# =============================================
# INITIAL WELCOME DIALOG
# =============================================

func _show_welcome_dialog():
	var canvas = CanvasLayer.new()
	canvas.name = "WelcomeCanvas"
	add_child(canvas)

	var bg = ColorRect.new()
	bg.color = UI_LOADING_BG_COLOR
	bg.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	canvas.add_child(bg)

	var label = Label.new()
	label.text = "Magic Carpet 2 setup required.\nPlease select the game installation folder."
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	label.offset_bottom = -80
	label.add_theme_font_size_override("font_size", 24)
	label.add_theme_color_override("font_color", UI_TEXT_COLOR)
	canvas.add_child(label)

	var btn = Button.new()
	btn.text = "SELECT FOLDER"
	btn.set_anchors_and_offsets_preset(Control.PRESET_CENTER_BOTTOM)
	btn.offset_top = -120
	btn.offset_bottom = -60
	btn.offset_left = -100
	btn.offset_right = 100
	_apply_button_theme(btn)

	btn.pressed.connect(func():
		canvas.queue_free()
		_show_custom_dir_picker()
	)
	canvas.add_child(btn)

# =============================================
# CUSTOM DIRECTORY PICKER
# =============================================

func _show_custom_dir_picker():
	_picker_canvas = CanvasLayer.new()
	_picker_canvas.name = "PickerCanvas"
	add_child(_picker_canvas)

	var bg = ColorRect.new()
	bg.color = Color(0, 0, 0, 0.7)
	bg.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_picker_canvas.add_child(bg)

	var panel = Panel.new()
	_picker_canvas.add_child(panel)
	
	var screen_size = get_viewport().get_visible_rect().size
	panel.custom_minimum_size = screen_size * 0.8
	panel.set_anchors_and_offsets_preset(Control.PRESET_CENTER)
	panel.offset_left = -(panel.custom_minimum_size.x / 2)
	panel.offset_top = -(panel.custom_minimum_size.y / 2)
	panel.offset_right = (panel.custom_minimum_size.x / 2)
	panel.offset_bottom = (panel.custom_minimum_size.y / 2)

	var panel_style = StyleBoxFlat.new()
	panel_style.bg_color = UI_BG_COLOR
	panel_style.border_color = UI_BORDER_COLOR
	panel_style.set_border_width_all(UI_BORDER_WIDTH)
	panel_style.set_corner_radius_all(UI_CORNER_RADIUS)
	panel.add_theme_stylebox_override("panel", panel_style)

	var margin = MarginContainer.new()
	margin.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	margin.add_theme_constant_override("margin_top", 20)
	margin.add_theme_constant_override("margin_bottom", 20)
	margin.add_theme_constant_override("margin_left", 20)
	margin.add_theme_constant_override("margin_right", 20)
	panel.add_child(margin)

	var vbox = VBoxContainer.new()
	vbox.add_theme_constant_override("separation", 10)
	margin.add_child(vbox)

	var title = Label.new()
	title.text = "SELECT INSTALLATION FOLDER"
	title.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	title.add_theme_color_override("font_color", UI_TITLE_COLOR)
	title.add_theme_font_size_override("font_size", UI_TITLE_FONT_SIZE)
	vbox.add_child(title)

	_picker_path_label = Label.new()
	_picker_path_label.text = "Path: /"
	_picker_path_label.add_theme_color_override("font_color", Color(0.5, 0.5, 0.6))
	_picker_path_label.add_theme_font_size_override("font_size", 14)
	vbox.add_child(_picker_path_label)

	_picker_tree = Tree.new()
	_picker_tree.size_flags_vertical = Control.SIZE_EXPAND_FILL
	_picker_tree.hide_root = true
	_apply_tree_theme(_picker_tree)
	_picker_tree.item_activated.connect(_on_picker_item_activated)
	_picker_tree.item_selected.connect(_on_picker_item_selected)
	vbox.add_child(_picker_tree)

	var hbox = HBoxContainer.new()
	hbox.alignment = BoxContainer.ALIGNMENT_END
	hbox.add_theme_constant_override("separation", 15)
	vbox.add_child(hbox)

	var btn_cancel = Button.new()
	btn_cancel.text = "CANCEL"
	btn_cancel.custom_minimum_size = Vector2(100, 40)
	_apply_button_theme(btn_cancel)
	btn_cancel.pressed.connect(func(): _picker_canvas.queue_free())
	hbox.add_child(btn_cancel)

	var btn_select = Button.new()
	btn_select.text = "SELECT CURRENT FOLDER"
	btn_select.custom_minimum_size = Vector2(200, 40)
	_apply_button_theme(btn_select)
	btn_select.pressed.connect(_on_picker_select_pressed)
	hbox.add_child(btn_select)

	var start_path = "C:/" if OS.get_name() == "Windows" else "/"
	_picker_navigate(start_path)

func _picker_navigate(path: String):
	_picker_current_path = path
	_picker_selected_path = path
	_picker_path_label.text = "Current Path: " + path
	_picker_tree.clear()

	var root = _picker_tree.create_item()
	var normalized = path.replace("\\", "/").trim_suffix("/")
	
	if normalized != "" and normalized != "C:":
		var up_item = _picker_tree.create_item(root)
		up_item.set_text(0, " .. (Go Back)")
		up_item.set_metadata(0, {"type": "up", "path": path.get_base_dir()})
		up_item.set_custom_color(0, Color(0.5, 0.6, 1.0))

	var dir = DirAccess.open(path)
	if dir == null: return

	dir.list_dir_begin()
	var folders: Array = []
	var file_name = dir.get_next()
	while file_name != "":
		if dir.current_is_dir() and not file_name.begins_with("."):
			folders.append(file_name)
		file_name = dir.get_next()
	dir.list_dir_end()
	folders.sort()

	for folder in folders:
		var full_path = path.trim_suffix("/") + "/" + folder
		var item = _picker_tree.create_item(root)
		item.set_text(0, " 📁 " + folder)
		item.set_metadata(0, {"type": "dir", "path": full_path})

func _on_picker_item_activated():
	var selected = _picker_tree.get_selected()
	if not selected: return
	var meta = selected.get_metadata(0)
	if meta["type"] == "up":
		_picker_navigate(_picker_current_path.get_base_dir())
	else:
		_picker_navigate(meta["path"])

func _on_picker_item_selected():
	var selected = _picker_tree.get_selected()
	if not selected: return
	var meta = selected.get_metadata(0)
	_picker_selected_path = meta["path"] if meta["type"] == "dir" else _picker_current_path

func _on_picker_select_pressed():
	var path = _picker_selected_path if _picker_selected_path != "" else _picker_current_path
	_picker_canvas.queue_free()
	_start_extraction(path)

# =============================================
# EXTRACTION & LOADING UI
# =============================================

func _start_extraction(dir: String):
	_show_loading_ui()
	_thread = Thread.new()
	_thread.start(_heavy_work.bind(dir))

func _heavy_work(dir):
	Global.MBEX = MBEXclass.new()
	Main_DecodeLevel.MBEXextractCD(Global.cdPath, dir)
	Global.MBEX.REMC2BeginGame(Global.cdPath, Global.hidata)
	Main_DecodeLevel.MBEXconvert(Global.convertdata, dir)
	call_deferred("_on_work_done")

func _on_work_done():
	_thread.wait_to_finish()
	Global.canNotification = true
	#Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED_HIDDEN)
	await _run_ending()
	OS.create_process(OS.get_executable_path(), OS.get_cmdline_args())
	get_tree().quit()

func _show_loading_ui():
	_stop_spinner = false # Reset flag
	var canvas = CanvasLayer.new()
	canvas.name = "LoadingCanvas"
	add_child(canvas)

	var bg = ColorRect.new()
	bg.color = UI_LOADING_BG_COLOR
	bg.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	canvas.add_child(bg)

	var label = Label.new()
	label.name = "StatusLabel"
	label.text = "Extracting and converting game data...\nPlease wait."
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	label.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	label.offset_bottom = -80
	label.add_theme_font_size_override("font_size", 26)
	label.add_theme_color_override("font_color", UI_TEXT_COLOR)
	canvas.add_child(label)

	var spinner = Label.new()
	spinner.name = "Spinner"
	spinner.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	spinner.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	spinner.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	spinner.offset_top = 100
	spinner.add_theme_font_size_override("font_size", 48)
	spinner.add_theme_color_override("font_color", UI_SPINNER_COLOR)
	canvas.add_child(spinner)
	_animate_spinner(spinner)

func _animate_spinner(spinner: Label):
	var frames = ["⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"]
	var i = 0
	# The loop now checks for the _stop_spinner flag
	while is_instance_valid(spinner) and not _stop_spinner:
		spinner.text = frames[i % frames.size()]
		i += 1
		await get_tree().create_timer(0.1).timeout

func _run_ending() -> void:
	_stop_spinner = true # Disable the animation loop
	var canvas = get_node("LoadingCanvas")
	var label = canvas.get_node("StatusLabel")
	label.text = "Data extraction complete.\nThank you for your patience.\nThe application will now restart."
	var countdown_label = canvas.get_node("Spinner")
	countdown_label.add_theme_color_override("font_color", UI_COUNTDOWN_COLOR)
	countdown_label.add_theme_font_size_override("font_size", 48)
	for i in range(5, -1, -1):
		countdown_label.text = str(i)
		await get_tree().create_timer(1.0).timeout

# =============================================
# THEME HELPERS
# =============================================

func _apply_button_theme(btn: Button):
	btn.add_theme_stylebox_override("normal",  _make_btn_style(UI_BTN_NORMAL_COLOR, UI_BTN_BORDER_COLOR))
	btn.add_theme_stylebox_override("hover",   _make_btn_style(UI_BTN_HOVER_COLOR,  UI_BTN_HOVER_BORDER))
	btn.add_theme_stylebox_override("pressed", _make_btn_style(UI_BTN_PRESS_COLOR,  UI_BTN_BORDER_COLOR))
	btn.add_theme_color_override("font_color", UI_BTN_TEXT_COLOR)

func _apply_tree_theme(tree: Tree):
	var style = StyleBoxFlat.new()
	style.bg_color = UI_TREE_BG_COLOR
	style.set_border_width_all(1)
	style.border_color = UI_TREE_BORDER_COLOR
	tree.add_theme_stylebox_override("panel", style)
	tree.add_theme_stylebox_override("focus", StyleBoxEmpty.new())
	var sel = StyleBoxFlat.new()
	sel.bg_color = UI_TREE_SELECT_COLOR
	tree.add_theme_stylebox_override("selected", sel)
	tree.add_theme_stylebox_override("selected_focus", sel)
	tree.add_theme_color_override("font_color", UI_TREE_TEXT_COLOR)
	tree.add_theme_color_override("font_selected_color", UI_TEXT_SELECT_COLOR)
	tree.add_theme_constant_override("v_separation", 4)

func _make_btn_style(bg: Color, border: Color) -> StyleBoxFlat:
	var s = StyleBoxFlat.new()
	s.bg_color = bg
	s.border_color = border
	s.set_border_width_all(2)
	s.set_corner_radius_all(4)
	return s
