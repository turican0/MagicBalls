extends Node3D

var _load_dialog: FileDialog = null
var _save_dialog: FileDialog = null

@onready var Main_DecodeLevel: Node3D = $DecodeLevel
@onready var MBEngine: Node3D = $MBEngine
@onready var Main_TerrainsMB: Node3D = $TerrainsMB
@onready var Main_Player: Node3D = $PlayerEditor
@onready var Main_Camera: Camera3D = $PlayerEditor/Camera3D
@onready var Ray_Cylinder: MeshInstance3D = $RayCylinder

@onready var Tree_View: Tree = $UI/TreeView/PreContainer/PanelContainer/MarginContainer/Tree

@onready var Terrain_Edit_Panel: Control = $UI/TerrainEdit
@onready var Entity_Edit_Panel: Control = $UI/EntityEdit
@onready var Wizards_Edit_Panel: Control = $UI/WizardsEdit
@onready var Stages_Edit_Panel: Control = $UI/StagesEdit
@onready var StagesVars_Edit_Panel: Control = $UI/StagesVarsEdit

@onready var Console: RichTextLabel = $UI/Console/RichTextLabel

@onready var Terrain_Edit = $UI/TerrainEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer
@onready var Entity_Edit: Control = $UI/EntityEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer
@onready var Wizards_Edit: Control = $UI/WizardsEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer
@onready var Stages_Edit: Control = $UI/StagesEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer
@onready var StagesVars_Edit: Control = $UI/StagesVarsEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer

@onready var EntityFilter: Control = $UI/EntityFilter

@onready var Position_Label: Control = $UI/Position/Label

@onready var selectors = [
	Terrain_Edit.get_node("seed"),
	Terrain_Edit.get_node("offset"),
	Terrain_Edit.get_node("raise"),
	Terrain_Edit.get_node("gnarl"),
	Terrain_Edit.get_node("river"),
	Terrain_Edit.get_node("lriver"),
	Terrain_Edit.get_node("source"),
	Terrain_Edit.get_node("snLin"),
	Terrain_Edit.get_node("snFlt"),
	Terrain_Edit.get_node("bhLin"),
	Terrain_Edit.get_node("bhFlt"),
	Terrain_Edit.get_node("rkSte"),
	Terrain_Edit.get_node("next")
]

@onready var TEselector2 = [
	Terrain_Edit.get_node("word_2FECE"),
	Terrain_Edit.get_node("levelID"),
	Terrain_Edit.get_node("byte_2FED2"),
	Terrain_Edit.get_node("byte_2FED3"),
	Terrain_Edit.get_node("map_type"),
	Terrain_Edit.get_node("word_2FED5"),
	Terrain_Edit.get_node("word_2FED7")
]

var editor_runned=false
var is_ui_visible = true

var roof_show = false

func _ready() -> void:
	await get_tree().process_frame
	Global.MBEX = MBEXclass.new()
	Main_DecodeLevel.NodeSky3D = $NodeSky3D/Sky3D
	Main_Camera.Ray_Cylinder = Ray_Cylinder
	Main_Camera.terrain_node = Main_TerrainsMB
	Main_Camera.editor = self
	var Ray_Cylinder: MeshInstance3D = null
	Global.setLevelType("Night")
	Global.MBEX.REMC2SetLevelType("Night")
	Main_TerrainsMB.init(false)
	Main_DecodeLevel.gameInit(false)
	gameInit()
	EditorInit(Global.cdPath)
	
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	#Main_TerrainsMB.updateMeshes(false)
	#fillTerrainDetails()
	_connect_terrain_spinboxes()
	
	#SetPlayerValues(Global.MBEX.REMC2EditorGetTerrainPlayers())
	#SetStagesValues(Global.MBEX.REMC2EditorGetTerrainStages())

	toggle_editor_control_styleSt(true)
	$UI.visible=true
	_preload_library(library, library_scenes)
	
	RenderEditorEntites()
	
	Tree_View.item_selected.connect(_on_tree_item_selected)
	
	get_window().focus_entered.connect(_on_window_focus_entered)
	get_window().focus_exited.connect(_on_window_focus_exited)
	
	get_window().mouse_entered.connect(_on_window_focus_entered)
	get_window().mouse_exited.connect(_on_window_focus_exited)
	
	#Wizards_Edit.display_player_data(0)
	#Stages_Edit.display_stages_data(0)
	_connect_entity_spinboxes()
	_connect_wizards_spinboxes()
	_connect_stages_spinboxes()
	_connect_stagesvars_spinboxes()
	
	SetRoofByVar()
	editor_runned=true
	
	var loader = get_tree().root.get_node_or_null("GlobalLoadingCanvas")
	if loader:
		loader.queue_free()
		
	#Tree_View.set_focus_mode(Control.FOCUS_ALL)
	#Tree_View.hide_folding = false
	
func _connect_terrain_spinboxes() -> void:
	for i in range(selectors.size()):
		var sb = selectors[i].get_node_or_null("SpinBox")
		if sb and not sb.value_changed.is_connected(_on_h_box_container_value_changed):
			sb.value_changed.connect(_on_h_box_container_value_changed.bind(i))

	for i in range(TEselector2.size()):
		var sb = TEselector2[i].get_node_or_null("SpinBox")
		if sb and not sb.value_changed.is_connected(_on_h_box_container_value_changed2):
			sb.value_changed.connect(_on_h_box_container_value_changed2.bind(i))

var _filling_terrain_details := false
func fillTerrainDetails() -> void:
	_filling_terrain_details = true
	for i in range(selectors.size()):
		var selector = selectors[i]
		var val = Global.editorLevel[selector.name]
		selector.current_value = val
		selector.get_node_or_null("SpinBox").value = val
	
	for i in range(TEselector2.size()):
		var selector = TEselector2[i]
		selector.get_node_or_null("SpinBox").value = Global.editorLevel[selector.name]
	_filling_terrain_details = false

var _has_focus := false
func DetectFocus() -> void:
	var window_focused = get_window().has_focus()
	if window_focused != _has_focus:
		_has_focus = window_focused
		if window_focused:
			_on_window_focus_entered()
		else:
			_on_window_focus_exited()

var _waiting_for_click_focus := false
func _on_window_focus_entered() -> void:
	if not is_ui_visible:
		_waiting_for_click_focus = true
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)

func _on_window_focus_exited() -> void:
	_waiting_for_click_focus = false
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	
#func SetPlayerValues(data: PackedFloat32Array):
	#if data.is_empty():
		#push_error("Žádná data z MBEX nebyla přijata!")
		#return
	#Global.players_settings.clear()
	#var floats_per_player = 86
	#var num_players = data.size() / floats_per_player
	#for i in range(num_players):
		#var wizard = Global.WizardSettings.new()
		#var offset = i * floats_per_player
		#wizard.aggression = int(data[offset + 0])
		#wizard.reflexes   = int(data[offset + 1])
		#wizard.perception = int(data[offset + 2])
		#for s in range(26):
			#wizard.starting_spells[s] = int(data[offset + 3 + s])
		#for s in range(26):
			#wizard.unknown_bytes[s]   = int(data[offset + 3 + 26 + s])
		#for s in range(26):
			#wizard.blocked_spells[s]  = int(data[offset + 3 + 26 + 26 + s])
		#wizard.life = int(data[offset + 81])
		#
		#wizard.stageIndex = int(data[offset + 82])
		#wizard.stageStage = int(data[offset + 83])
		#wizard.stageX = int(data[offset + 84])
		#wizard.stageY = int(data[offset + 85])
		#
		#Global.players_settings.append(wizard)

#func SetStagesValues(data: PackedFloat32Array):
	#if data.is_empty():
		#push_error("Žádná data z MBEX nebyla přijata!")
		#return
	#Global.stages_settings.clear()
	#var floats_per_stage = 6
	#var num_stages = data.size() / floats_per_stage
	#for i in range(num_stages):
		#var stage = Global.StagesSettings.new()
		#var offset = i * floats_per_stage
		#stage.index = int(data[offset + 0])
		#stage.stage = int(data[offset + 1])
		#stage.x1 = int(data[offset + 2])
		#stage.y1 = int(data[offset + 3])
		#stage.x2 = int(data[offset + 4])
		#stage.y2 = int(data[offset + 5])
		#
		#Global.stages_settings.append(stage)

func _move_tree_selection(direction: int) -> void:
	var root = Tree_View.get_root()
	if root == null:
		return
	var section_title = ""
	var current_idx = -1
	if Entity_Edit_Panel.visible:
		section_title = "Entities"
		var entities = get_tree().get_nodes_in_group("entities")
		current_edited_entity=clamp(current_edited_entity + direction, 1, entities.size())
	elif Wizards_Edit_Panel.visible:
		section_title = "Wizards"
		current_edited_wizard = clamp(current_edited_wizard + direction, 0, 7)
	elif Stages_Edit_Panel.visible:
		section_title = "Stages"
		current_edited_stage = clamp(current_edited_stage + direction, 0, 7)
	elif StagesVars_Edit_Panel.visible:
		section_title = "StagesVars"
		current_edited_stagevar = clamp(current_edited_stagevar + direction, 0, 11)
	else:
		section_title = "Terrain"
	
func _input(event: InputEvent) -> void:
	if _waiting_for_click_focus:
		if event is InputEventMouseButton and not event.pressed:
			_waiting_for_click_focus = false
			Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
			return
			
	if Tree_View.has_focus() and event is InputEventKey and event.pressed:
		if event.keycode == KEY_UP:
			_move_tree_selection(-1)
			get_viewport().set_input_as_handled()
			return
		elif event.keycode == KEY_DOWN:
			_move_tree_selection(1)
			get_viewport().set_input_as_handled()
			return

	# M pressed
	if (event is InputEventKey and event.keycode == KEY_H and event.pressed):
		$UI.visible = !$UI.visible
	if (event is InputEventKey and event.keycode == KEY_X and event.pressed):
		delete_selected_entities()
	if (event is InputEventKey and event.keycode == KEY_N and event.pressed):
		add_entitity()
	# UNDO & REDO
	if event is InputEventKey and event.pressed:
		var command_or_ctrl = event.ctrl_pressed or event.meta_pressed
		if command_or_ctrl and event.keycode == KEY_Z:
			if event.shift_pressed: # CTRL+SHIFT+Z maybe as redo
				_on_redo_button_down()
			else:
				_on_undo_button_down()
		if command_or_ctrl and event.keycode == KEY_Y:
			_on_redo_button_down()
	if event is InputEventMouseButton and event.pressed:
		match event.button_index:
			MOUSE_BUTTON_MIDDLE:
				toggle_editor_control_style()
	if !is_ui_visible:
		if event is InputEventMouseButton:
			match event.button_index:
				MOUSE_BUTTON_WHEEL_UP:
					Main_Camera._cylinder_radius = clamp(Main_Camera._cylinder_radius + Main_Camera.cylinder_scroll_step, Main_Camera.cylinder_radius_min, Main_Camera.cylinder_radius_max)
				MOUSE_BUTTON_WHEEL_DOWN:
					Main_Camera._cylinder_radius = clamp(Main_Camera._cylinder_radius - Main_Camera.cylinder_scroll_step, Main_Camera.cylinder_radius_min, Main_Camera.cylinder_radius_max)

func toggle_editor_control_style():
	is_ui_visible = !is_ui_visible
	toggle_editor_control_styleSt(is_ui_visible)
		
func toggle_editor_control_styleSt(state):
	if state:
		Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
		#Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED)
	else:
		Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

var _last_scrolled_entity: int = -99
var _last_scrolled_wizard: int = -99
var _last_scrolled_stage: int = -99
var _last_scrolled_stagevar: int = -99
func update_tree():
	var all_sections: Array = []

	# ── Terrain selectors ───────────────────────────────────────
	var terrain_items = []
	for entity in TEselector2:
		if is_instance_valid(entity):
			var sb = entity.get_node_or_null("SpinBox")
			terrain_items.append({
				"name": str(entity.name),
				"value": str(sb.value if sb else "?"),  #UPDATE
				"id": 0
			})
	for entity in selectors:
		if is_instance_valid(entity):
			terrain_items.append({
				"name": str(entity.name),
				"value": str(entity.current_value),
				"id": entity.name.get_slice("_", 1).to_int()
			})
	all_sections.append({ "title": "Terrain", "items": terrain_items })

	# ── Entities ────────────────────────────────────────────────
	var entity_items = []
	for entity in get_tree().get_nodes_in_group("entities"):
		if is_instance_valid(entity):
			entity_items.append({
				"name": "E:" + str(entity.get_meta("index")) + " T:" + str(entity.get_meta("type_0x30311")) + " S:" + str(entity.get_meta("subtype_0x30311")),
				"value": str(entity.current_value if "current_value" in entity else entity.position),
				"id": entity.get_meta("index", -1)   # lepší použít meta "index", které tam ukládáš
			})
	entity_items.sort_custom(func(a, b): return a["id"] < b["id"])
	all_sections.append({ "title": "Entities", "items": entity_items })
	
	# ── Wizards (Spells) ────────────────────────────────────────
	var spells_items = [] #UPDATE
	var wizards: Array = Global.editorLevel.get("wizards", []) #UPDATE
	for i in range(wizards.size()): #UPDATE
		var w = wizards[i] #UPDATE
		spells_items.append({ #UPDATE
			"name": "W%d  Agg:%d  Ref:%d  Per:%d  Life:%d" % [i, w.get("aggression",0), w.get("reflexes",0), w.get("perception",0), w.get("life",0)], #UPDATE
			"value": str(i), #UPDATE
			"id": i #UPDATE
		}) #UPDATE
	all_sections.append({ "title": "Wizards", "items": spells_items })

	# ── Stages ──────────────────────────────────────────────────
	var stages_items = [] #UPDATE
	var stages: Array = Global.editorLevel.get("stages", []) #UPDATE
	for i in range(stages.size()): #UPDATE
		var s = stages[i] #UPDATE
		stages_items.append({ #UPDATE
			"name": "S%d  Idx:%d  Stg:%d  X:%d  Y:%d" % [i, s.get("index",0), s.get("stage",0), s.get("axis_x",0), s.get("axis_y",0)], #UPDATE
			"value": str(i), #UPDATE
			"id": i #UPDATE
		}) #UPDATE
	all_sections.append({ "title": "Stages", "items": stages_items })

	# ── StageVars ───────────────────────────────────────────────
	var stagesvars_items = [] #UPDATE
	var stage_vars: Array = Global.editorLevel.get("stage_vars", []) #UPDATE
	for i in range(stage_vars.size()): #UPDATE
		var v = stage_vars[i] #UPDATE
		stagesvars_items.append({ #UPDATE
			"name": "SV%d  Idx:%d  Stg:%d  X1:%d  Y1:%d  X2:%d  Y2:%d" % [i, v.get("index",0), v.get("stage",0), v.get("union_axis_2d_x",0), v.get("union_axis_2d_y",0), v.get("union_dword_axis_x",0), v.get("union_dword_axis_y",0)], #UPDATE
			"value": str(i), #UPDATE
			"id": i #UPDATE
		}) #UPDATE
	all_sections.append({ "title": "StagesVars", "items": stagesvars_items })
	
	Tree_View.update_tree_view(all_sections)
	
	_highlight_active_and_selected_items()
	
	if Entity_Edit_Panel.visible and current_edited_entity >= 0:
		if current_edited_entity != _last_scrolled_entity:
			_last_scrolled_entity = current_edited_entity
			_on_tree_entity_selected(current_edited_entity)
			_scroll_tree_to_active("Entities", current_edited_entity)
	elif Wizards_Edit_Panel.visible and current_edited_wizard >= 0:
		if current_edited_wizard != _last_scrolled_wizard:
			_last_scrolled_wizard = current_edited_wizard
			_on_tree_wizard_selected(current_edited_wizard)
			_scroll_tree_to_active("Wizards", current_edited_wizard)
	elif Stages_Edit_Panel.visible and current_edited_stage >= 0:
		if current_edited_stage != _last_scrolled_stage:
			_last_scrolled_stage = current_edited_stage
			_on_tree_stages_selected(current_edited_stage)
			_scroll_tree_to_active("Stages", current_edited_stage)
	elif StagesVars_Edit_Panel.visible and current_edited_stagevar >= 0:
		if current_edited_stagevar != _last_scrolled_stagevar:
			_last_scrolled_stagevar = current_edited_stagevar
			_on_tree_stagesVars_selected(current_edited_stagevar)
			_scroll_tree_to_active("StagesVars", current_edited_stagevar)

func _scroll_tree_to_active(section_title: String, item_index: int) -> void:
	var root = Tree_View.get_root()
	if root == null:
		return
	var section = root.get_first_child()
	while section:
		if section.get_text(0) == section_title:
			break
		section = section.get_next()
	if section == null:
		return
	var item = section.get_first_child()
	while item:
		if item.get_metadata(0) == item_index:
			Tree_View.scroll_to_item(item)
			return
		item = item.get_next()
		
func _highlight_active_and_selected_items() -> void:
	var root = Tree_View.get_root()
	if root == null:
		return
	var selected_3d = {}
	for node in get_tree().get_nodes_in_group("selected_entities"):
		if node.has_meta("index"):
			selected_3d[node.get_meta("index")] = true
	var section = root.get_first_child()
	while section:
		var title = section.get_text(0)
		var item = section.get_first_child()
		while item:
			var id = item.get_metadata(0) as int
			var is_active = false
			var is_3d_sel = id in selected_3d
			match title:
				"Entities":
					is_active = (id == current_edited_entity)
				"Wizards":
					is_active = (id == current_edited_wizard)
				"Stages":
					is_active = (id == current_edited_stage)
				"StagesVars":
					is_active = (id == current_edited_stagevar)
			if is_active and is_3d_sel:
				item.set_custom_bg_color(0, Color(0.0, 0.85, 0.95, 0.65))   # Azurová / Cyan
			elif is_active:
				item.set_custom_bg_color(0, Color(0.25, 0.6, 1.0, 0.6))     # Modrá
			elif is_3d_sel:
				item.set_custom_bg_color(0, Color(1.0, 0.9, 0.2, 0.45))     # Žlutá
			else:
				item.clear_custom_bg_color(0)
			item = item.get_next()
		section = section.get_next()
		
func update_selection() -> void:
	var ray_nodes = get_tree().get_nodes_in_group("selected_entities_ray")
	var filter_nodes = get_tree().get_nodes_in_group("selected_entities_filter")
	for node in get_tree().get_nodes_in_group("selected_entities"):
		unmark_as_selected(node,"selected_entities")
	if not Ray_Cylinder.visible and not EntityFilter_On:
		for node in get_tree().get_nodes_in_group("entities"):
			mark_as_selected(node,"selected_entities")
		return

	# Výpočet průniku
	var final_selection: Array[Node] = []

	if Ray_Cylinder.visible and EntityFilter_On:
		# PRŮNIK obou
		for node in ray_nodes:
			if node in filter_nodes:           # nebo node.is_in_group("selected_entities_filter")
				final_selection.append(node)
				
	elif Ray_Cylinder.visible:
		# Pouze ray/cylinder
		final_selection = ray_nodes.duplicate()
		
	elif EntityFilter_On:
		# Pouze filter
		final_selection = filter_nodes.duplicate()

	# Aplikujeme finální výběr
	for node in final_selection:
		if is_instance_valid(node):
			mark_as_selected(node,"selected_entities")
			
	if FirstSelectedToEdit and not final_selection.is_empty():
		if is_instance_valid(final_selection[0]):
			var node = final_selection[0]
			if node.has_meta("index"):
				fillEntityDetails(node.get_meta("index"))
	
	# Debug výpis (můžeš smazat)
	# print("Selection updated: %d entities | Cylinder: %s | Filter: %s" % [selected_count, cylinder_active, filter_active])

func _process(delta: float) -> void:
	if editor_runned:
		EditorStep()
		Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
		#fillTerrainDetails()
		#refreshWizardDetails()
		update_tree()
		Global.MBEX.REMC2EditorTimedSaveState(1.0)
		UpdatePositionLabel()
		select_entities_by_filter()
		update_selection()
		RenderEditorEntites()

func gameInit():
	match Global.getLevelType():
		"Day":
			Main_DecodeLevel.setTime(9.0)
			Main_DecodeLevel.setDayEntites()
			Main_DecodeLevel.setSkyExposure(0.3)
			Main_DecodeLevel.sefFogEnd(200)
			Main_DecodeLevel.setFogFall(15)
			Main_DecodeLevel.setFogDensity(0.01)
			Main_DecodeLevel.setAtmDayTint(Color(0.8,0.9,1.0))
			Main_DecodeLevel.setSunMoon(true,true,0.7,1.0)
		"Night":
			Main_DecodeLevel.setTime(2.5)
			Main_DecodeLevel.setNightEntites()
			Main_DecodeLevel.setSkyExposure(1.5)
			Main_DecodeLevel.sefFogEnd(200)
			Main_DecodeLevel.setFogFall(15)
			Main_DecodeLevel.setFogDensity(0.01)
			Main_DecodeLevel.setAtmDayTint(Color(0.8,0.9,1.0))
			Main_DecodeLevel.setSunMoon(true,true,1.0,1.0)
		"Cave":
			Main_DecodeLevel.setTime(0.0)
			Main_DecodeLevel.setFog(0.03)
			Main_DecodeLevel.setFogSky(1.0)
			Main_DecodeLevel.setSkyExposure(0.3)
			Main_DecodeLevel.setCaveEntites()
			Main_DecodeLevel.setAtmDayTint(Color(0,0,0))
			Main_DecodeLevel.setSunMoon(false,true,1.0,0.0)
		"Final":
			Main_DecodeLevel.setTime(2.5)
			Main_DecodeLevel.setNightEntites()
			Main_DecodeLevel.setSkyExposure(1.5)
			Main_DecodeLevel.sefFogEnd(200)
			Main_DecodeLevel.setFogFall(15)
			Main_DecodeLevel.setFogDensity(0.01)
			Main_DecodeLevel.setAtmDayTint(Color(0.8,0.9,1.0))
			Main_DecodeLevel.setSunMoon(true,true,1.0,1.0)

func EditorInit(cdPath):
	Global.MBEX.REMC2EditorBegin(cdPath)

var updateRoof:bool = false

func EditorStep():
	Global.MBEX.REMC2EditorLoop()
	Global.MBEX.renew_terrain(updateRoof)

var pool_size = 1200

var library_scenes = {}

func _preload_library(source_dict: Dictionary, target_dict: Dictionary):
	for key in source_dict:
		var path = source_dict[key]
		if path != "":
			target_dict[key] = load(path)

func updateLibrary(a:int,b:int,c:int,path:String):
	library[Vector3i(a, b, c)] = path
	library_scenes[Vector3i(a, b, c)] = load(path)

var library = {
	Vector3i(2,0,0): "res://entites-editor/object_2_0_tree.tscn",
	Vector3i(2,2,0): "res://entites-editor/object_2_2_dolmen.tscn",
	Vector3i(3,4,0): "res://entites-editor/object_3_4_player.tscn",
	Vector3i(5,1,0): "res://entites-editor/object_5_1_goat.tscn",
	Vector3i(5,3,0): "res://entites-editor/object_5_3_worm.tscn",
	Vector3i(5,4,0): "res://entites-editor/object_5_4_archer.tscn",
	Vector3i(5,13,0): "res://entites-editor/object_5_13_people.tscn",
	Vector3i(5,19,0): "res://entites-editor/object_5_19_firefly.tscn",
	Vector3i(14,5,0): "res://entites-editor/object_14_5_scroll.tscn",
	Vector3i(15,2,0): "res://entites-editor/object_15_2_vase.tscn",
	Vector3i(10,45,0): "res://entites-editor/object_10_45_house.tscn",
	Vector3i(10,0,0): "res://entites-editor/object_10_0_explosion.tscn",
	Vector3i(10,1,0): "res://entites-editor/object_10_1_big_explosion.tscn",
	Vector3i(10,59,0): "res://entites-editor/object_10_59_smoke1.tscn",
	Vector3i(10,60,0): "res://entites-editor/object_10_60_smoke2.tscn",
	Vector3i(0,993,0): "res://entites-editor/object_arrow_stage.tscn",
	Vector3i(0,994,0): "res://entites-editor/object_stageVEditor.tscn",
	Vector3i(0,995,0): "res://entites-editor/object_stageEditor.tscn",
	Vector3i(0,996,0): "res://entites-editor/object_arrow_disid.tscn",  # parent arrow
	Vector3i(0,997,0): "res://entites-editor/object_arrow_path.tscn",  # parent arrow
	Vector3i(0,998,0): "res://entites-editor/object_arrow_parent.tscn",  # parent arrow
	Vector3i(0,1000,0): "res://entites-editor/object_textEditor.tscn",
	#Vector3i(2,75,0): "res://entites/object_2_75_tree.tscn",#tree -difColors!!!
	#Vector3i(2,78,0): "res://entites/object_2_78_statue.tscn",#statue -difmodels!!!
	#Vector3i(2,79,0): "res://entites/object_2_79_dolmen.tscn",#dolmen
	#Vector3i(2,87,0): "res://entites/object_2_87_tree.tscn",#tree2
	#Vector3i(2,178,0): "res://entites/object_2_178_burned_tree.tscn",#burned tree
	#Vector3i(2,179,0): "res://entites/object_2_179_burned_tree.tscn",#burned tree
	#Vector3i(2,198,0): "res://entites/object_2_78_statue.tscn",#statue2 - level2 - same models
	#Vector3i(2,422,0): "res://entites/object_2_422_barell.tscn",#barell
	#Vector3i(2,423,0): "res://entites/object_2_423_basket.tscn",#basket - yyyyyyyyyyyyyyyyyyyyyyyyy
	#Vector3i(2,424,0): "res://entites/object_2_424_mushroom1.tscn",#mushroom1 - zzzzzzzzzzzzzzzzzzzzzzzzzz
	#Vector3i(2,425,0): "res://entites/object_2_425_mushroom2.tscn",#mushroom1 - zzzzzzzzzzzzzzzzzzzzzzzzzz
	#Vector3i(3,0,0): "",#player1-Zanzamar
	#Vector3i(3,88,0): "res://entites/object_3_88_ballon.tscn",#ballon -difColors!!!
	#Vector3i(3,89,0): "res://entites/object_3_89_ballon.tscn",#ballon -difColors!!!
	#Vector3i(3,90,0): "res://entites/object_3_90_ballon.tscn",#ballon -difColors!!!
	#Vector3i(3,91,0): "res://entites/object_3_91_ballon.tscn",#ballon -difColors!!!
	#Vector3i(3,92,0): "res://entites/object_3_92_ballon.tscn",#ballon -difColors!!!
	#Vector3i(3,93,0): "res://entites/object_3_93_ballon.tscn",#ballon -difColors!!!
	#Vector3i(3,94,0): "res://entites/object_3_94_ballon.tscn",#ballon -difColors!!!
	#Vector3i(3,95,0): "res://entites/object_3_95_ballon.tscn",#ballon -difColors!!!
	#Vector3i(3,96,0): "res://entites/object_3_96_posses_building-whiteD.tscn",#castle -difmodels!!!
	#Vector3i(3,97,0): "res://entites/object_3_97_posses_building-redD.tscn",#castle -difmodels!!!
	#Vector3i(3,98,0): "res://entites/object_3_98_posses_building-violetD.tscn",#castle -difmodels!!!
	#Vector3i(3,99,0): "res://entites/object_3_99_posses_building-blueD.tscn",#castle -difmodels!!!
	#Vector3i(3,100,0): "res://entites/object_3_100_posses_building-greenD.tscn",#castle -difmodels!!!
	#Vector3i(3,101,0): "res://entites/object_3_101_posses_building-pinkD.tscn",#castle -difmodels!!!
	#Vector3i(3,102,0): "res://entites/object_3_102_posses_building-orangeD.tscn",#castle -difmodels!!!
	#Vector3i(3,103,0): "res://entites/object_3_103_posses_building-blackD.tscn",#castle -difmodels!!!
	#Vector3i(3,203,0): "res://entites/object_3_203_sorcerer-blue.tscn",
	#Vector3i(3,211,0): "res://entites/object_3_211_sorcerer-red.tscn",#sorcerer-red xxxxxxxxxxxxxxxxxxxx
	#Vector3i(3,219,0): "res://entites/object_3_219_sorcerer-black.tscn",#sorcerer-black xxxxxxxxxxxxxxxxxxxx
	#Vector3i(3,227,0): "res://entites/object_3_227_sorcerer-orange.tscn",#sorcerer-orange xxxxxxxxxxxxxxxxxxxx
	#Vector3i(3,235,0): "res://entites/object_3_235_sorcerer-pink.tscn",#sorcerer-pink xxxxxxxxxxxxxxxxxxxx
	#Vector3i(3,243,0): "res://entites/object_3_243_sorcerer-violet.tscn",#sorcerer-violet xxxxxxxxxxxxxxxxxxxx
	#Vector3i(3,251,0): "res://entites/object_3_251_sorcerer-green.tscn",#sorcerer-green xxxxxxxxxxxxxxxxxxxx
	#Vector3i(5,8,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,9,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,10,0): "res://entites/object_5_8_bowman.tscn",#bowman
	#Vector3i(5,11,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,12,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,13,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow
	#Vector3i(5,14,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	#Vector3i(5,15,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	#Vector3i(5,16,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	#Vector3i(5,17,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	#Vector3i(5,18,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	#Vector3i(5,19,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	#Vector3i(5,20,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	#Vector3i(5,21,0): "res://entites/object_5_11_bowman.tscn",#bowman-arrow -difmodels!!!
	#Vector3i(5,22,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	#Vector3i(5,23,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	#Vector3i(5,24,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	#Vector3i(5,25,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	#Vector3i(5,26,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	#Vector3i(5,27,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	#Vector3i(5,28,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	#Vector3i(5,29,0): "res://entites/object_5_26_flyingCentipedeHead.tscn",#xxxxxxxxxxxxxxxxxxx
	#Vector3i(5,30,0): "res://entites/object_5_30_centipedeHead.tscn",#centipede-head-OK
	#Vector3i(5,56,0): "res://entites/object_5_56_flyingCentipedeBody.tscn",#xxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(5,55,0): "res://entites/object_9_55_fireball.tscn",#fireball-OK-make as star-special
	#Vector3i(5,58,0): "res://entites/object_10_58_goldSphere.tscn",#fliing goldMana -OK
	#Vector3i(5,66,0): "res://entites/object_5_66_centipedeBody.tscn",#centipede-body-OK
	#Vector3i(5,67,0): "res://entites/object_10_67_whiteSphere.tscn",
	#Vector3i(5,68,0): "res://entites/object_10_68_redSphere.tscn",
	#Vector3i(5,69,0): "res://entites/object_10_69_violedSphere.tscn",#violet sphere xxxxxxxxxxxx
	#Vector3i(5,70,0): "res://entites/object_10_70_blueSphere.tscn",#blue sphere xxxxxxxxxxxx
	#Vector3i(5,71,0): "res://entites/object_10_71_greenSphere.tscn",#green sphere xxxxxxxxxxxx
	#Vector3i(5,72,0): "res://entites/object_10_72_pinkSphere.tscn",#pink sphere xxxxxxxxxxxx
	#Vector3i(5,73,0): "res://entites/object_10_73_orangeSphere.tscn",#orange sphere xxxxxxxxxxxx
	#Vector3i(5,74,0): "res://entites/object_10_74_blackSphere.tscn",#black sphere xxxxxxxxxxxx
	#Vector3i(5,110,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	#Vector3i(5,111,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	#Vector3i(5,112,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	#Vector3i(5,113,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	#Vector3i(5,114,0): "res://entites/object_5_110_vampire_bowman.tscn",#vampire archer
	#Vector3i(5,121,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch-OK
	#Vector3i(5,122,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch-OK
	#Vector3i(5,123,0): "res://entites/object_5_121_bowman.tscn",#bowman-crouch-OK
	#Vector3i(5,124,0): "res://entites/object_5_124_dragon.tscn",#dragon
	#Vector3i(5,132,0): "res://entites/object_5_132_people5.tscn",#people5 132 133 134-OK
	#Vector3i(5,135,0): "res://entites/object_5_135_people6.tscn",#people6 135 136 137-OK
	#Vector3i(5,138,0): "res://entites/object_5_138_people7.tscn",#people7 138 139 140-OK
	#Vector3i(5,141,0): "res://entites/object_5_141_people8.tscn",#people8 141 142 143-OK
	#Vector3i(5,152,0): "res://entites/object_5_152_goat.tscn",#goat-OK
	#Vector3i(5,155,0): "res://entites/object_5_155_people1.tscn",#people1 155 156 157-OK
	#Vector3i(5,158,0): "res://entites/object_5_158_puerla.tscn",#puerla xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(5,180,0): "res://entites/object_5_180_people2.tscn",#people2 180 181 182-OK
	#Vector3i(5,183,0): "res://entites/object_5_183_people3.tscn",#people3 183 184 185-OK
	#
	#Vector3i(5,271,0): "res://entites/object_5_271_stoneHead.tscn",#stone head
	#Vector3i(5,295,0): "res://entites/object_5_295_crabBaloon.tscn",#crab baloon
	#Vector3i(5,303,0): "res://entites/object_5_303_manticore.tscn",# 5-303-gryf/manticore
	#
	#Vector3i(5,328,0): "res://entites/object_5_328_hopper.tscn",#328-335	levá noha pokrčena v koleni, nahoře	ctvrta
	#Vector3i(5,336,0): "res://entites/object_5_336_hopper.tscn",#336-343	pravá noha mírně za levou, obě na zemi	pata
	#Vector3i(5,344,0): "res://entites/object_5_344_hopper.tscn",#344-351	pravá noha mírně za levou, obě na zemi, tělo mírně přikrčené	sesta
	#Vector3i(5,352,0): "res://entites/object_5_352_hopper.tscn",#352-359	pravá noha mírně za levou, obě na zemi, tělo více přikrčené	první
	#Vector3i(5,360,0): "res://entites/object_5_360_hopper.tscn",#360-367	levá noha před pravou v nákroku, obě na zemi	druha
	#Vector3i(5,368,0): "res://entites/object_5_368_hopper.tscn",#368-375	levá noha pokrčena v koleni, nahoře, pravá téměř natažena	treti
	#Vector3i(5,376,0): "res://entites/object_5_376_hopper.tscn",#376-383	nohy kousek od sebe stojací postoj	
	#Vector3i(5,384,0): "res://entites/object_5_384_hopper.tscn",#376-383	torso
	#Vector3i(5,392,0): "res://entites/object_5_392_minibasket.tscn",#mini
	#Vector3i(5,393,0): "res://entites/object_5_393_minibasket.tscn",#mini-torso
	#Vector3i(5,199,0): "res://entites/object_5_199_people4.tscn",#people4 199 200 201-OK
	#Vector3i(5,263,0): "res://entites/object_5_263_darklion.tscn",#lion zzzzzzzzzzzz
	#Vector3i(5,279,0): "res://entites/object_5_279_beetle.tscn",#beetle-OK
	#Vector3i(5,287,0): "res://entites/object_5_287_spider.tscn",#spider zzzzzzzzzzzz
	#
	#Vector3i(5,311,0): "res://entites/object_5_311_water_beast.tscn",#water beast
	#Vector3i(5,319,0): "res://entites/object_5_319_water_beast.tscn",#water beast torso
	#
	#Vector3i(5,394,0): "res://entites/object_5_394_bigDragonBody.tscn",#bigDragon-body-move
	#Vector3i(5,402,0): "res://entites/object_5_402_bigDragonHead.tscn",#bigDragon-head
	#Vector3i(5,410,0): "res://entites/object_5_410_bigDragonNeck.tscn",#bigDragon-neck
	#
	#Vector3i(5,411,0): "res://entites/object_5_411_zombie.tscn",#zombie
	#
	#Vector3i(5,437,0): "res://entites/object_5_437_mummy.tscn",#mummy-walk
	#Vector3i(5,445,0): "res://entites/object_5_445_mummy.tscn",#mummy-stay
	#
	#Vector3i(5,453,0): "res://entites/object_5_453_bigDragonBody.tscn",#bigDragon-body-stay
	#
	#Vector3i(5,464,0): "res://entites/object_5_464_vissuluth.tscn",#vissuluth
	#Vector3i(5,472,0): "res://entites/object_5_472_vissuluth.tscn",#vissuluth
	#Vector3i(5,480,0): "res://entites/object_5_480_vissuluth.tscn",#vissuluth
	#Vector3i(5,488,0): "res://entites/object_5_488_vissuluth.tscn",#vissuluth
	#Vector3i(5,496,0): "res://entites/object_5_496_vissuluth.tscn",#vissuluth
	#
	#Vector3i(9,55,0): "res://entites/object_9_55_fireball.tscn",#fireball-OK-make as star
	#Vector3i(9,60,0): "res://entites/object_9_60_green-spell.tscn",#green-spell
	#Vector3i(9,61,0): "res://entites/object_9_61_castleball.tscn",#castleball-OK
	#Vector3i(9,64,0): "res://entites/object_9_64_meteor.tscn",#meteor-OK
	#Vector3i(9,80,0): "res://entites/object_10_80_castle_orb.tscn",#fire xxxxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(9,105,0): "res://entites/object_9_105_arrow.tscn",#arrow-OK
	#Vector3i(9,116,0): "res://entites/object_9_116_firearrow.tscn",#arrow xxxxxxxxxxxxxxxxx
	#Vector3i(9,144,0): "res://entites/object_9_144_posses.tscn",#posses-OK - more star
	#Vector3i(9,148,0): "res://entites/object_9_148_red_hand.tscn",#red hand-OK
	#Vector3i(9,149,0): "res://entites/object_9_149_violet_orb.tscn",#violet orb-OK
	#Vector3i(9,151,0): "res://entites/object_9_151_lighting.tscn",#fix this to better
	#Vector3i(9,419,0): "res://entites/object_9_419_spider_web.tscn",#spide web zzzzzzzzzzzzzzOK
	#Vector3i(9,420,0): "res://entites/object_9_420_walnut.tscn",#walnul ball zzzzzzzzzzzzzzOK
	#Vector3i(9,421,0): "res://entites/object_9_421_red_sphere.tscn",#red sphere-OK
	#Vector3i(9,145,0): "res://entites/object_9_146_lump.tscn",#tremor ball zzzzzzzzzzzzzzOK
	#Vector3i(9,146,0): "res://entites/object_9_146_lump.tscn",#crater ball zzzzzzzzzzzzzzOK
	#Vector3i(9,260,0): "res://entites/object_9_260_bone.tscn",#bone spell zzzzzzzzzzzzzzOK
	#Vector3i(9,463,0): "res://entites/object_9_463_sunball.tscn",#sunball-OK
	#Vector3i(10,8,0): "res://entites/object_10_8_fair.tscn",#fair-fake number 8 not true index of model xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(10,38,0): "res://entites/object_10_8_fair.tscn",#end-of explosion-OK
	#Vector3i(10,54,0): "res://entites/object_10_54_explosion.tscn",#explosion-final-OK
	#Vector3i(10,55,0): "res://entites/object_9_55_fireball.tscn",#fireball-OK-make as star-special
	#Vector3i(10,57,0): "res://entites/object_10_57_smoke.tscn",#smoke1-OK
	#Vector3i(10,58,0): "res://entites/object_10_58_goldSphere.tscn",#goldMana -OK-standartMana
	#Vector3i(10,63,0): "res://entites/object_10_63_smoke.tscn",#smoke2 -OK
	#Vector3i(10,67,0): "res://entites/object_10_67_whiteSphere.tscn",#whiteMana -OK
	#Vector3i(10,68,0): "res://entites/object_10_68_redSphere.tscn",#red sphere xxxxxxxxxxxx
	#Vector3i(10,69,0): "res://entites/object_10_69_violedSphere.tscn",#violet sphere xxxxxxxxxxxx
	#Vector3i(10,70,0): "res://entites/object_10_70_blueSphere.tscn",#blue sphere xxxxxxxxxxxx
	#Vector3i(10,71,0): "res://entites/object_10_71_greenSphere.tscn",#green sphere xxxxxxxxxxxx
	#Vector3i(10,72,0): "res://entites/object_10_72_pinkSphere.tscn",#pink sphere xxxxxxxxxxxx
	#Vector3i(10,73,0): "res://entites/object_10_73_orangeSphere.tscn",#orange sphere xxxxxxxxxxxx
	#Vector3i(10,74,0): "res://entites/object_10_74_blackSphere.tscn",#black sphere xxxxxxxxxxxx	
	#Vector3i(10,77,0): "res://entites/object_10_77_fire.tscn",#fire xxxxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(10,80,0): "res://entites/object_10_80_castle_orb.tscn",#fire xxxxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(10,81,0): "res://entites/object_10_81_remains.tscn",#remains xxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(10,96,0): "res://entites/object_3_96_posses_building-whiteD.tscn",#building -difmodels!!!
	#Vector3i(10,97,0): "res://entites/object_3_97_posses_building-redD.tscn",#building -difmodels!!!
	#Vector3i(10,98,0): "res://entites/object_3_98_posses_building-violetD.tscn",#building -difmodels!!!
	#Vector3i(10,99,0): "res://entites/object_3_99_posses_building-blueD.tscn",#building -difmodels!!!
	#Vector3i(10,100,0): "res://entites/object_3_100_posses_building-greenD.tscn",#building -difmodels!!!
	#Vector3i(10,101,0): "res://entites/object_3_101_posses_building-pinkD.tscn",#building -difmodels!!!
	#Vector3i(10,102,0): "res://entites/object_3_102_posses_building-orangeD.tscn",#building -difmodels!!!
	#Vector3i(10,103,0): "res://entites/object_3_103_posses_building-blackD.tscn",#building -difmodels!!!
	#Vector3i(10,145,0): "res://entites/object_9_64_meteor.tscn",#meteor
	#Vector3i(10,158,0): "res://entites/object_5_158_puerla.tscn",#puerla xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(10,168,0): "res://entites/object_10_168_FacePortal.tscn",#fire xxxxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(10,186,0): "res://entites/object_10_186_splash.tscn",#splash -difmodels!!! - in cave buble
	#Vector3i(10,202,0): "res://entites/object_10_202_blue_orb.tscn",#blue orb
	#Vector3i(10,327,0): "res://entites/object_10_327_tornado.tscn",#tornado xxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(10,426,0): "res://entites/object_10_426_bubble.tscn",#bubble - zzzzzzzzzzzzzzzzzzzzzzzzzz
	#Vector3i(10,463,0): "res://entites/object_10_463_fireball-object.tscn",#fireball-object xxxxxxxxxxxxxxxxxxxxxxxxxx
	#Vector3i(14,259,0): "res://entites/object_14_259_scroll.tscn",#scroll -OK
	#Vector3i(14,461,0): "res://entites/object_14_461_mouth.tscn",#mouth-gate -OK
	#Vector3i(14,462,0): "res://entites/object_14_462_portal.tscn",#day portal
	#Vector3i(15,59,0): "res://entites/object_15_59_jar.tscn",#jar -OK
}

var entites_pool:Dictionary

func add_to_entites_pool(uid: Vector3i, sendNode: Node) -> void:
	if not entites_pool.has(uid):
		entites_pool[uid] = {
			"array": [],
			"active_count": 0,
			"act_index": 0
		}
	entites_pool[uid]["array"].append(sendNode)
	entites_pool[uid]["active_count"] += 1

func get_first_entity_with_uid(uid: Vector3i) -> Node:
	if entites_pool.has(uid) and not entites_pool[uid]["array"].is_empty():
		if(entites_pool[uid]["act_index"]<entites_pool[uid]["array"].size()):
			var result = entites_pool[uid]["array"][entites_pool[uid]["act_index"]]
			return result
		else:
			return null;
	return null

func add_pool_index(uid: Vector3i):
	if entites_pool.has(uid) and not entites_pool[uid]["array"].is_empty():
		entites_pool[uid]["act_index"]+=1
		entites_pool[uid]["active_count"] += 1

func show_hide_entites() -> void:
	for bucket in entites_pool.values():
		var arr = bucket["array"]
		var active_count = bucket["active_count"]
		for i in range(arr.size()):
			var node: Node = arr[i]
			if i < active_count:
				node.show()
				node.set_process(true)
				node.set_physics_process(true)
			else:
				node.hide()
				node.set_process(false)
				node.set_physics_process(false)
				#if(node.get_meta("uid")==Vector3i(14,461,0) or node.get_meta("uid")==Vector3i(14,462,0)):#remove entites with start script
					#node.queue_free()
					#arr.remove_at(i)
		if arr.is_empty():
			entites_pool.erase(bucket)
		bucket["act_index"] = 0
		bucket["active_count"] = 0

func clear_entites_pool() -> void:
	for bucket in entites_pool.values():
		var arr = bucket["array"]
		for node in arr:
			if is_instance_valid(node):
				node.queue_free()
	entites_pool.clear()

static var _next_uid: int = 0
func generate_unique_id() -> int:
	_next_uid += 1
	return _next_uid

func select_entities_in_radius_2D(center_pos_3d: Vector3, radius: float):
	if is_ui_visible:
		return
	var center_2d = Vector2(center_pos_3d.x, center_pos_3d.z)
	var radius_squared = radius * radius
	
	var found_any_in_radius = false
	var closest_node = null
	var min_dist_sq = INF # Nastavíme na nekonečno
	
	# 1. Průchod: Zjistíme co je v kruhu a najdeme nejbližšího kandidáta
	if Ray_Cylinder.visible:
		for node in get_tree().get_nodes_in_group("entities"):
			var node_pos_2d = Vector2(node.global_position.x, node.global_position.z)
			var dist_sq = center_2d.distance_squared_to(node_pos_2d)
			unmark_as_selected(node,"selected_entities_ray")
			if dist_sq < min_dist_sq:
				min_dist_sq = dist_sq
				closest_node = node
			if dist_sq <= radius_squared:
				mark_as_selected(node,"selected_entities_ray")
				found_any_in_radius = true
		if not found_any_in_radius and closest_node != null:
			mark_as_selected(closest_node,"selected_entities_ray")

func select_entities_by_filter():
	if !EntityFilter_On:
		return
	for node in get_tree().get_nodes_in_group("entities"):
		var should_be_selected = true
		for filter in Global.editorFilteres:
			if node.has_meta(filter.filterName):
				if node.get_meta(filter.filterName) != filter.filterKey:
					should_be_selected = false
					break
			else:
				should_be_selected = false
				break
		if should_be_selected:
			mark_as_selected(node,"selected_entities_filter")
		else:
			unmark_as_selected(node,"selected_entities_filter")

func mark_as_selected(node: Node3D,type:String):
	if node.is_in_group(type): return
	node.add_to_group(type)#"selected_entities"
	if(type=="selected_entities"):
		var mesh = node.get_node_or_null("MeshInstance3D")
		if mesh:
			var mat = mesh.get_surface_override_material(0)
			if not mat:
				mat = mesh.mesh.surface_get_material(0).duplicate()
				mesh.set_surface_override_material(0, mat)
			mat.albedo_color = Color(1.0, 1.0, 0.0)

func unmark_as_selected(node: Node3D,type:String):
	if not node.is_in_group(type): return
	node.remove_from_group(type)
	if(type=="selected_entities"):
		var mesh = node.get_node_or_null("MeshInstance3D")
		if mesh:
			var mat = mesh.get_surface_override_material(0)
			if mat:
				mat.albedo_color = Color(1, 1, 1) 

func add_entitity():
	var selected_nodes = get_tree().get_nodes_in_group("selected_entities")
	var copy_node: Dictionary={}
	if(current_edited_entity>-1):
		copy_node=Global.editorLevel["entities"][current_edited_entity]
	else:
		if !selected_nodes.is_empty():
			copy_node=Global.editorLevel["entities"][selected_nodes[0].get_meta("index")]
	Global.MBEX.REMC2EditorAddEntity(copy_node)

func delete_selected_entities():
	var selected_nodes = get_tree().get_nodes_in_group("selected_entities")
	if selected_nodes.is_empty():
		print("Nic není označeno ke smazání.")
		return
	var myArray:Array
	for node in selected_nodes:
		if node.has_meta("index"):
			var index = node.get_meta("index")
			myArray.append(index)
	var result = Global.MBEX.REMC2EditorDeleteEntites(myArray)
	var nexttext=""
	if !(result & 8):
		nexttext = "Deleted entites: " + str(selected_nodes.size())
	if (result & 2):
		nexttext = nexttext + " some entities have childrens, delete itx, too"
	if (result & 4):
		nexttext = nexttext + " some entities not deleted, have parent"
	log_message(nexttext)
	EditorStep()
	#RenderEditorEntites()

func RenderEditorEntites():
	for node in get_tree().get_nodes_in_group("entities"):
		node.remove_from_group("entities")
	var entites_per_frame=0;
	#var stride = 11
	var inv_256 = 1.0 / 256.0
	var camera = get_viewport().get_camera_3d()
	var has_camera = camera != null
	var cam_pos = camera.global_position if has_camera else Vector3.ZERO
	var rad_mult = PI / 1024.0 # Zjednodušeno z PI / (256 * 4)
	var default_key = Vector3i(0, 1000, 0)
	for i in range(1, pool_size):
		#var offset = i * stride
		var type_0x30311 = Global.editorLevel["entities"][i]["type"]
		var subtype_0x30311 = Global.editorLevel["entities"][i]["subtype"]
		var pos = Vector3(Global.editorLevel["entities"][i]["axis_x"], Global.editorLevel["entities"][i]["axis_z"], Global.editorLevel["entities"][i]["axis_y"])
		var DisId = Global.editorLevel["entities"][i]["dis_id"]
		var word_10 = Global.editorLevel["entities"][i]["word10"]
		var stageTag_12 = Global.editorLevel["entities"][i]["stage_tag"]
		var par1_14 = Global.editorLevel["entities"][i]["par1"]
		var par2_16 = Global.editorLevel["entities"][i]["par2"]
		var par3_18 = Global.editorLevel["entities"][i]["par3"]
		
		var updateObject=false
		var current_node = null
		var fromlib = false
		var uid2 = Vector3i(type_0x30311,subtype_0x30311,0)
		var scene_to_instance = null
		var libType:int = 0
		
		libType = 1
		if library_scenes.has(uid2):
			scene_to_instance = library_scenes[uid2]
			fromlib = true
		elif not library.has(uid2):
			scene_to_instance = library_scenes.get(default_key)
		if(type_0x30311==0)&&(subtype_0x30311==0):
			scene_to_instance = null
		if scene_to_instance != null:
			current_node = get_first_entity_with_uid(uid2)
			if current_node == null:
				var new_node = scene_to_instance.instantiate()
				new_node.set_meta("uid",uid2)
				add_child(new_node)
				current_node = new_node
				add_to_entites_pool(uid2,new_node)
				updateObject=true
			else:
				add_pool_index(uid2)
				updateObject=true
		if (current_node&&updateObject):
			if current_node.get_node_or_null("Label3D"):
					current_node.get_node("Label3D").text = (
						"IDX: %d\n" +
						"T: %d, " +
						"ST: %d\n" +
						"Pos: %d,%d\n" +
						"DisId: %d, " +
						"W10: %d, " +
						"Stage: %d\n" +
						"Par1: %d, " +
						"Par2: %d, " +
						"Par3: %d"
					) % [
						i,
						type_0x30311, subtype_0x30311,
						pos.x,pos.z,
						DisId, word_10, stageTag_12, 
						par1_14, par2_16, par3_18
					]
			
			current_node.set_meta("index",int(i))
			
			current_node.set_meta("axis_x", int(pos.x))
			current_node.set_meta("axis_y", int(pos.z))
			current_node.set_meta("axis_z", int(pos.y))
			
			current_node.set_meta("type_0x30311",int(type_0x30311))
			current_node.set_meta("subtype_0x30311",int(subtype_0x30311))
			current_node.set_meta("DisId",int(DisId))
			current_node.set_meta("word_10",int(word_10))
			current_node.set_meta("stageTag_12",int(stageTag_12))
			current_node.set_meta("par1_14",int(par1_14))
			current_node.set_meta("par2_16",int(par2_16))
			current_node.set_meta("par3_18",int(par3_18))
			
			current_node.add_to_group("entities")
			var entityScale = 1.0
			current_node.scale = Vector3(entityScale, entityScale, entityScale)
			var base_pos_x = pos.x# * inv_256
			var base_pos_y = pos.y# * inv_256
			var base_pos_z = pos.z# * inv_256
			#if has_camera:
				#var new_x = cam_pos.x + fposmod(base_pos_x - cam_pos.x + 128.0, 256.0) - 128.0
				#var new_z = cam_pos.z + fposmod(base_pos_z - cam_pos.z + 128.0, 256.0) - 128.0
				#current_node.global_position = Vector3(new_x, base_pos_y, new_z)
			#else:
			current_node.position = Vector3(base_pos_x, base_pos_y, base_pos_z)
			#var yaw = -rot2.x * rad_mult
			#current_node.rotation = Vector3(0, yaw, 0)
	AddParentsArrows()
	AddPathArrows()
	
	AddStages()
	AddStagesVars()
	#AddDisIdArrows()
	AddDisId2Arrows()
	show_hide_entites()

func AddStages():
	var x = -2
	var y = 0
	var z = 2
	for stages in Global.editorLevel["stages"]:
		var index = stages["index"]
		var stage = stages["stage"]
		var axis_x = stages["axis_x"]
		var axis_y = stages["axis_y"]
		
		y=y+ 1
		
		var pos = Vector3(x, z, y)
		
		var current_node = null
		var updateObject: bool = false
		var uid2=Vector3i(0,995,0)
		if(index == -1):
			continue
		current_node = get_first_entity_with_uid(uid2)
		if current_node == null:
			var scene_to_instance = library_scenes[uid2]
			var new_node = scene_to_instance.instantiate()
			add_child(new_node)
			current_node = new_node
			add_to_entites_pool(uid2,new_node)
			updateObject=true
		else:
			add_pool_index(uid2)
			updateObject=true
		if (current_node&&updateObject):
			if current_node.get_node_or_null("Label3D"):
					current_node.get_node("Label3D").text = (
						"index: %d\n" +
						"stage: %d\n" +
						"axis_x: %d\n" +
						"axis_y: %d"
					) % [
						index,
						stage,
						axis_x,
						axis_y
					]
			if(index == 7):
				var entity = Global.editorLevel["entities"][stage]
				pos = Vector3(entity["axis_x"], entity["axis_z"], entity["axis_y"])
			current_node.position = Vector3(pos.x, pos.y, pos.z)
		
	
func AddStagesVars():
	var x = -4
	var y = 0
	var z = 2
	for stages in Global.editorLevel["stage_vars"]:
		var index = (stages["index"]+256) % 256
		var indexA = index % 128
		var indexB = int(index / 128)
		var stage = stages["stage"]
		var union_axis_2d_x = stages["union_axis_2d_x"]
		var union_axis_2d_y = stages["union_axis_2d_y"]
		var union_dword_axis_x = stages["union_dword_axis_x"]
		var union_dword_axis_y = stages["union_dword_axis_y"]
		
		y=y+ 1
		
		var pos = Vector3(x, z, y)
		
		var current_node = null
		var updateObject: bool = false
		var uid2=Vector3i(0,994,0)
		
		var current_nodeB = null
		var updateObjectB: bool = false
		var uid2B=Vector3i(0,993,0)
		if(indexA == 0):
			continue
		current_node = get_first_entity_with_uid(uid2)
		if current_node == null:
			var scene_to_instance = library_scenes[uid2]
			var new_node = scene_to_instance.instantiate()
			add_child(new_node)
			current_node = new_node
			add_to_entites_pool(uid2,new_node)
			updateObject=true
		else:
			add_pool_index(uid2)
			updateObject=true
		if current_node && (indexA==1 || indexA==2):
			current_nodeB = get_first_entity_with_uid(uid2B)
			if current_nodeB == null:
				var scene_to_instanceB = library_scenes[uid2B]
				var new_nodeB = scene_to_instanceB.instantiate()
				add_child(new_nodeB)
				current_nodeB = new_nodeB
				add_to_entites_pool(uid2B,new_nodeB)
				updateObjectB=true
			else:
				add_pool_index(uid2B)
				updateObjectB=true
			
		if (current_node&&updateObject):
			if current_node.get_node_or_null("Label3D"):
					current_node.get_node("Label3D").text = (
						"indexes: %d,%d\n" +
						"stage: %d\n" +
						"axis_x: %d, " +
						"axis_y: %d\n" +
						"axis_x2: %d, " +
						"axis_y2: %d"
					) % [
						indexA,indexB,
						stage,
						union_axis_2d_x,
						union_axis_2d_y,
						union_dword_axis_x,
						union_dword_axis_y
					]
			if(union_axis_2d_x > 0) && (indexA==1 || indexA==2):
				var entity = Global.editorLevel["entities"][union_axis_2d_x]
				pos = Vector3(entity["axis_x"], entity["axis_z"], entity["axis_y"])
			current_node.position = Vector3(pos.x, pos.y, pos.z)
		if (current_nodeB&&updateObjectB):
			var zAxis=Global.MBEX.REMC2GetTerrainAlt(union_dword_axis_x,union_dword_axis_y)
			current_nodeB.get_node("Arrow").end_pos = Vector3(union_dword_axis_x, zAxis + 4, union_dword_axis_y)
			if(union_axis_2d_x == 0):
				pos.x=union_dword_axis_x
				pos.y=zAxis
				pos.z=union_dword_axis_y
				current_node.position = Vector3(pos.x, pos.y, pos.z)
				current_nodeB.get_node("Arrow").start_pos = Vector3(current_node.position.x, current_node.position.y + 5, current_node.position.z)
			else:
				current_nodeB.get_node("Arrow").start_pos = Vector3(current_node.position.x, current_node.position.y + 4, current_node.position.z)
			

func AddParentsArrows():
	var index_to_pos: Dictionary = {}
	for entity in get_tree().get_nodes_in_group("entities"):
		index_to_pos[entity.get_meta("index")] = entity.position
		
	for entity in get_tree().get_nodes_in_group("entities"):
		var current_node = null
		var updateObject: bool = false
		if !Global.MBEX.REMC2EditorIsParentType(entity.get_meta("type_0x30311"),entity.get_meta("subtype_0x30311")):
			continue
		if entity.get_meta("par1_14") in index_to_pos:
			var parent_index = entity.get_meta("par1_14")
			var uid2=Vector3i(0,998,0)
			current_node = get_first_entity_with_uid(uid2)
			if current_node == null:
				var scene_to_instance = library_scenes[uid2]
				var new_node = scene_to_instance.instantiate()
				add_child(new_node)
				current_node = new_node
				add_to_entites_pool(uid2,new_node)
				updateObject=true
			else:
				add_pool_index(uid2)
				updateObject=true
			var parent_pos = index_to_pos.get(parent_index, null)
			current_node.set_meta("start", entity.position+Vector3(0,2,0))
			current_node.set_meta("end", parent_pos+Vector3(0,2,0))
		if (current_node&&updateObject):
			current_node.get_node("Arrow").start_pos = current_node.get_meta("start")
			current_node.get_node("Arrow").end_pos = current_node.get_meta("end")

func AddPathArrows():
	var index_to_pos: Dictionary = {}
	for entity in get_tree().get_nodes_in_group("entities"):
		index_to_pos[entity.get_meta("index")] = entity.position
		
	for entity in get_tree().get_nodes_in_group("entities"):
		var current_node = null
		var updateObject: bool = false
		if entity.get_meta("par2_16") in index_to_pos:
			var path_index = entity.get_meta("par2_16")
			var uid2 = Vector3i(0,997,0)
			current_node = get_first_entity_with_uid(uid2)
			if current_node == null:
				var scene_to_instance = library_scenes[uid2]
				var new_node = scene_to_instance.instantiate()
				add_child(new_node)
				current_node = new_node
				add_to_entites_pool(uid2,new_node)
				updateObject = true
			else:
				add_pool_index(uid2)
				updateObject = true
			var path_pos = index_to_pos.get(path_index, null)
			current_node.set_meta("start", entity.position + Vector3(0, 2, 0))
			current_node.set_meta("end", path_pos + Vector3(0, 2, 0))
		if (current_node && updateObject):
			current_node.get_node("Arrow").start_pos = current_node.get_meta("start")
			current_node.get_node("Arrow").end_pos = current_node.get_meta("end")

func AddDisIdArrows():
	var disid_groups: Dictionary = {}
	for entity in get_tree().get_nodes_in_group("entities"):
		if Global.MBEX.REMC2EditorIsGroupType(entity.get_meta("type_0x30311"),entity.get_meta("subtype_0x30311")):
			var disid = entity.get_meta("DisId")
			if disid <= 0:
				continue
			if not disid_groups.has(disid):
				disid_groups[disid] = []
			disid_groups[disid].append(entity)
	for disid in disid_groups:
		var group = disid_groups[disid]
		if group.size() < 2:
			continue
		for i in range(group.size() - 1):
			var entity_a = group[i]
			var entity_b = group[i + 1]
			var current_node = null
			var updateObject: bool = false
			var uid2 = Vector3i(0, 997, 0)
			current_node = get_first_entity_with_uid(uid2)
			if current_node == null:
				var scene_to_instance = library_scenes[uid2]
				var new_node = scene_to_instance.instantiate()
				add_child(new_node)
				current_node = new_node
				add_to_entites_pool(uid2, new_node)
				updateObject = true
			else:
				add_pool_index(uid2)
				updateObject = true
			current_node.set_meta("start", entity_a.position + Vector3(0, 2, 0))
			current_node.set_meta("end", entity_b.position + Vector3(0, 2, 0))
			if current_node && updateObject:
				current_node.get_node("Arrow").start_pos = current_node.get_meta("start")
				current_node.get_node("Arrow").end_pos = current_node.get_meta("end")
				
func AddDisId2Arrows():
	var index_to_pos: Dictionary = {}
	for entity in get_tree().get_nodes_in_group("entities"):
		if(entity.get_meta("stageTag_12")>0) and (entity.get_meta("type_0x30311")==11):
			index_to_pos[entity.get_meta("stageTag_12")] = entity.position
		
	for entity in get_tree().get_nodes_in_group("entities"):
		var current_node = null
		var updateObject: bool = false
		#if(entity.get_meta("type_0x30311")==11):
			#continue
		#if !Global.MBEX.REMC2EditorIsParentType(entity.get_meta("type_0x30311"),entity.get_meta("subtype_0x30311")):
			#continue
		if entity.get_meta("DisId") in index_to_pos:
			var parent_index = entity.get_meta("DisId")
			var uid2=Vector3i(0,998,0)
			current_node = get_first_entity_with_uid(uid2)
			if current_node == null:
				var scene_to_instance = library_scenes[uid2]
				var new_node = scene_to_instance.instantiate()
				add_child(new_node)
				current_node = new_node
				add_to_entites_pool(uid2,new_node)
				updateObject=true
			else:
				add_pool_index(uid2)
				updateObject=true
			var parent_pos = index_to_pos.get(parent_index, null)
			current_node.set_meta("start", entity.position+Vector3(0,2,0))
			current_node.set_meta("end", parent_pos+Vector3(0,2,0))
		if (current_node&&updateObject):
			current_node.get_node("Arrow").start_pos = current_node.get_meta("end")
			current_node.get_node("Arrow").end_pos = current_node.get_meta("start")

func EditorEnd():
	Global.MBEX.REMC2EditorEnd()

func _on_h_box_container_value_changed(new_value: int,terrainVarIndex: int) -> void:
	if _filling_terrain_details:
		return
	Global.editorLevel[selectors[terrainVarIndex].name] = new_value
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)
	EditorStep()
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	#RenderEditorEntites()

func _on_h_box_container_value_changed2(new_value: int,terrainVarIndex: int) -> void:
	if _filling_terrain_details:
		return
	Global.editorLevel[TEselector2[terrainVarIndex].name] = new_value
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)

func _on_undo_button_down() -> void:
	Global.MBEX.REMC2EditorUndo()
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	print("testUndo"+str(Global.editorLevel["word_2FECE"]))
	_refresh_active_panel()
	log_message("UNDO")

func _on_redo_button_down() -> void:
	Global.MBEX.REMC2EditorRedo()
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	_refresh_active_panel()
	log_message("REDO")

func _refresh_active_panel() -> void:
	if Terrain_Edit_Panel.visible:
		fillTerrainDetails()
	elif Entity_Edit_Panel.visible:
		_save_current_entity_changes()
		var idx = Entity_Edit.get_node_or_null("IDX/SpinBox").value as int
		if idx > 0 and idx < pool_size:
			fillEntityDetails(idx)
	elif Wizards_Edit_Panel.visible:
		var idx = Wizards_Edit.get_node_or_null("IDX/SpinBox").value as int
		if idx >= 0 and idx <= 7:
			_filling_wizard_details = true
			Wizards_Edit.display_player_data(idx)
			_filling_wizard_details = false
	elif Stages_Edit_Panel.visible:
		var idx = Stages_Edit.get_node_or_null("IDX/SpinBox").value as int
		if idx >= 0 and idx <= 7:
			_filling_stages_details = true
			Stages_Edit.display_stages_data(idx)
			_filling_stages_details = false
	elif StagesVars_Edit_Panel.visible:
		var idx = StagesVars_Edit.get_node_or_null("IDX/SpinBox").value as int
		if idx >= 0 and idx <= 11:
			_filling_stagesvars_details = true
			StagesVars_Edit.display_stages_data(idx)
			_filling_stagesvars_details = false

func SaveState() -> void:
	Global.MBEX.REMC2EditorSaveState()

func log_message(text: String, color: String = "white"):
	var formatted_text = "[color=" + color + "]" + text + "[/color]\n"
	Console.append_text(formatted_text)
	var scrollbar = Console.get_v_scroll_bar()
	scrollbar.value = scrollbar.max_value

var _filling_entity_details := false
var _filling_wizard_details := false

var current_entity_index: int = -1

func fillEntityDetails(index:int):
	if index < 0 or index >= pool_size:
		return
	current_entity_index = index
	_filling_entity_details = true
	var entities = get_tree().get_nodes_in_group("entities")
	var finded_node = null
	for node in entities:
		if node.has_meta("index") and node.get_meta("index") == index:
			finded_node=node
	if finded_node:
		Entity_Edit.get_node_or_null("IDX/SpinBox").value = finded_node.get_meta("index")
		Entity_Edit.get_node_or_null("POS/SpinBox").value = finded_node.get_meta("axis_x")
		Entity_Edit.get_node_or_null("POS/SpinBox2").value = finded_node.get_meta("axis_y")
		Entity_Edit.get_node_or_null("type_0x30311/SpinBox").value = finded_node.get_meta("type_0x30311")
		Entity_Edit.get_node_or_null("subtype_0x30311/SpinBox").value = finded_node.get_meta("subtype_0x30311")
		Entity_Edit.get_node_or_null("DisId/SpinBox").value = finded_node.get_meta("DisId")
		Entity_Edit.get_node_or_null("word_10/SpinBox").value = finded_node.get_meta("word_10")
		Entity_Edit.get_node_or_null("stageTag_12/SpinBox").value = finded_node.get_meta("stageTag_12")
		Entity_Edit.get_node_or_null("par1_14/SpinBox").value = finded_node.get_meta("par1_14")
		Entity_Edit.get_node_or_null("par2_16/SpinBox").value = finded_node.get_meta("par2_16")
		Entity_Edit.get_node_or_null("par3_18/SpinBox").value = finded_node.get_meta("par3_18")
		_filling_entity_details = false
		
func _on_tree_item_selected() -> void:
	var selected = Tree_View.get_selected()
	if selected == null:
		return
	var parent = selected.get_parent()
	if parent == null:
		return
	if parent == Tree_View.get_root():
		return
		
	var section_title = parent.get_text(0)  # "Terrain" nebo "Entities"
	var item_text = selected.get_text(0)    # např. "Seed_0"
	var item_index: int = -1
	if selected.get_metadata(0) != null:
		item_index = selected.get_metadata(0)
	match section_title:
		"Terrain":
			_on_tree_terrain_selected(item_index)
		"Entities":
			_on_tree_entity_selected(item_index)
		"Wizards":
			_on_tree_wizard_selected(item_index)
		"Stages":
			_on_tree_stages_selected(item_index)
		"StagesVars":
			_on_tree_stagesVars_selected(item_index)
	#_refresh_active_panel()

var current_edited_entity: int = -1
var current_edited_wizard: int = -1
var current_edited_stage: int = -1
var current_edited_stagevar: int = -1

func _on_tree_terrain_selected(index: int) -> void:
	log_message("Terrain selected: index=%d" % [index])
	Terrain_Edit_Panel.show()
	Entity_Edit_Panel.hide()
	Wizards_Edit_Panel.hide()
	Stages_Edit_Panel.hide()
	StagesVars_Edit_Panel.hide()
	current_edited_stage = -1
	current_edited_entity = -1
	current_edited_wizard = -1
	current_edited_stagevar = -1
	fillTerrainDetails()

func _on_tree_entity_selected(index: int) -> void:
	log_message("Entity selected: index=%d" % [index])
	if(Entity_Edit_Panel.visible):
		_save_current_entity_changes()
	Terrain_Edit_Panel.hide()
	Entity_Edit_Panel.show()
	Wizards_Edit_Panel.hide()
	Stages_Edit_Panel.hide()
	StagesVars_Edit_Panel.hide()
	current_edited_entity = index
	current_edited_wizard = -1
	current_edited_stage = -1
	current_edited_stagevar = -1
	fillEntityDetails(index)
	
func _on_tree_wizard_selected(index: int):
	if Wizards_Edit_Panel.visible:
		_save_current_wizards_changes()
	Wizards_Edit_Panel.show()
	Terrain_Edit_Panel.hide()
	Entity_Edit_Panel.hide()
	Stages_Edit_Panel.hide()
	StagesVars_Edit_Panel.hide()
	current_edited_wizard = index
	current_edited_entity = -1
	current_edited_stage = -1
	current_edited_stagevar = -1
	_filling_wizard_details = true
	Wizards_Edit.display_player_data(index)
	_filling_wizard_details = false

func _on_tree_stages_selected(index: int):
	if Stages_Edit_Panel.visible:
		_save_current_stages_changes()
	Terrain_Edit_Panel.hide()
	Entity_Edit_Panel.hide()
	Wizards_Edit_Panel.hide()
	Stages_Edit_Panel.show()
	StagesVars_Edit_Panel.hide()
	current_edited_stage = index
	current_edited_entity = -1
	current_edited_wizard = -1
	current_edited_stagevar = -1
	_filling_stages_details = true
	Stages_Edit.display_stages_data(index)
	_filling_stages_details = false
 
func _on_tree_stagesVars_selected(index: int):
	if StagesVars_Edit_Panel.visible:
		_save_current_stagesvars_changes()
	Terrain_Edit_Panel.hide()
	Entity_Edit_Panel.hide()
	Wizards_Edit_Panel.hide()
	Stages_Edit_Panel.hide()
	StagesVars_Edit_Panel.show()
	current_edited_stagevar = index
	current_edited_entity = -1
	current_edited_wizard = -1
	current_edited_stage = -1
	_filling_stagesvars_details = true
	StagesVars_Edit.display_stages_data(index)
	_filling_stagesvars_details = false
	
const MENU_FILE_EXPORT_CSV = 0
const MENU_FILE_QLOAD_LEVEL = 1
const MENU_FILE_QSAVE_LEVEL = 2
const MENU_FILE_LOAD_LEVEL = 3
const MENU_FILE_SAVE_LEVEL = 4
const MENU_FILE_RUN_LEVEL = 5
const MENU_FILE_CLEAN_LEVEL = 6
const MENU_FILE_GAME_LEVEL = 7
const MENU_FILE_EXAMPLE_LEVEL = 8

const MENU_FILTER_SELECTRAY = 0
const MENU_FILTER_SELECTFILTER = 1
const MENU_FILTER_SHOWFILTER = 2
const MENU_FILTER_SELECTTOEDIT = 3

func _ensure_file_dialogs() -> void:
	var default_dir = ProjectSettings.globalize_path("user://user-levels/")
	if _load_dialog == null:
		_load_dialog = FileDialog.new()
		_load_dialog.file_mode = FileDialog.FILE_MODE_OPEN_FILE
		_load_dialog.access = FileDialog.ACCESS_FILESYSTEM
		_load_dialog.filters = PackedStringArray(["*.mc2 ; MC2 Level Files", "*.* ; All Files"])
		_load_dialog.title = "Load Level"
		_load_dialog.size = Vector2i(800, 600)
		_load_dialog.current_dir = default_dir
		_load_dialog.file_selected.connect(_on_load_dialog_file_selected)
		add_child(_load_dialog)

	if _save_dialog == null:
		_save_dialog = FileDialog.new()
		_save_dialog.file_mode = FileDialog.FILE_MODE_SAVE_FILE
		_save_dialog.access = FileDialog.ACCESS_FILESYSTEM
		_save_dialog.filters = PackedStringArray(["*.mc2 ; MC2 Level Files", "*.* ; All Files"])
		_save_dialog.title = "Save Level"
		_save_dialog.size = Vector2i(800, 600)
		_save_dialog.current_dir = default_dir
		_save_dialog.current_file = "level0.mc2"
		_save_dialog.file_selected.connect(_on_save_dialog_file_selected)
		add_child(_save_dialog)
		
func _on_load_dialog_file_selected(path: String) -> void:
	var result = Global.MBEX.REMC2EditorLoadLevel(path)
	if(!result):
		log_message("Can not Load this level file:" + path)
	else:
		Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
		fillTerrainDetails()
		#RenderEditorEntites()
		log_message("Level loaded: " + path)

func _on_save_dialog_file_selected(path: String) -> void:
	Global.MBEX.REMC2EditorSaveLevel(path)
	log_message("Level saved: " + path)
	
	
var _level_select_dialog: Window # Nebo AcceptDialog
func _ensure_level_select_dialog() -> void:
	if _level_select_dialog != null:
		return

	_level_select_dialog = Window.new()
	_level_select_dialog.title = "Select Game Level"
	_level_select_dialog.size = Vector2i(900, 700)
	_level_select_dialog.min_size = Vector2i(600, 500)
	_level_select_dialog.exclusive = true
	_level_select_dialog.transient = true
	_level_select_dialog.close_requested.connect(func(): _level_select_dialog.hide())
	
	var scroll = ScrollContainer.new()
	scroll.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT, Control.PRESET_MODE_MINSIZE, 15)
	
	var main_vbox = VBoxContainer.new()
	main_vbox.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	main_vbox.add_theme_constant_override("separation", 10) # Mezera mezi sekvencemi
	
	if Global.VALID_LEVELS.size() > 0:
		var current_hbox = HBoxContainer.new()
		main_vbox.add_child(current_hbox)
		
		for i in range(Global.VALID_LEVELS.size()):
			var lvl_id = Global.VALID_LEVELS[i]
			
			# Kontrola spojitosti: Pokud toto není první prvek a 
			# aktuální ID není o 1 větší než předchozí, založ nový řádek
			if i > 0 and Global.VALID_LEVELS[i] != Global.VALID_LEVELS[i-1] + 1:
				current_hbox = HBoxContainer.new()
				main_vbox.add_child(current_hbox)
			
			var btn = Button.new()
			btn.text = str(lvl_id)
			btn.custom_minimum_size = Vector2(40, 40)
			btn.pressed.connect(_on_level_button_pressed.bind(lvl_id))
			current_hbox.add_child(btn)

	scroll.add_child(main_vbox)
	_level_select_dialog.add_child(scroll)
	add_child(_level_select_dialog)

func _on_level_button_pressed(lvl_id: int) -> void:
	_level_select_dialog.hide()
	log_message("Loading Game Level: " + str(lvl_id))
	Global.MBEX.REMC2EditorLoadInGameLevel(lvl_id)
	# Po loadu herního levelu pravděpodobně chceš refreshnout editor:
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	_on_map_type_state_changed_graphics_typeInt(Global.editorLevel["map_type"])
	fillTerrainDetails()
	#RenderEditorEntites()

var example_names = [
	"Entities type 2",
	"Entities type 3",
	"Entities type 5",
	"Entities type 10",
	"Entities type 14",
	"Entities type 15",
	"empty",
	"empty",
	"empty",
	"empty",
	"empty"
]
var _example_select_dialog: Window
func _ensure_example_select_dialog() -> void:
	if _example_select_dialog != null:
		return
	_example_select_dialog = Window.new()
	_example_select_dialog.title = "Select Example Level"
	_example_select_dialog.size = Vector2i(500, 150)
	_example_select_dialog.min_size = Vector2i(400, 120)
	_example_select_dialog.exclusive = true
	_example_select_dialog.transient = true
	_example_select_dialog.close_requested.connect(func(): _example_select_dialog.hide())
	var vbox = VBoxContainer.new()
	vbox.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT, Control.PRESET_MODE_MINSIZE, 10)
	var label = Label.new()
	label.text = "Choose example (0–10):"
	vbox.add_child(label)
	var hbox = HBoxContainer.new()
	hbox.add_theme_constant_override("separation", 6)
	for i in range(11):
		var col = VBoxContainer.new()
		col.alignment = BoxContainer.ALIGNMENT_CENTER
		var btn = Button.new()
		btn.text = str(i)
		btn.custom_minimum_size = Vector2(40, 40)
		btn.pressed.connect(_on_example_button_pressed.bind(i))
		col.add_child(btn)
		var lbl = Label.new()
		lbl.text = example_names[i]
		lbl.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		lbl.add_theme_font_size_override("font_size", 10)
		col.add_child(lbl)
		hbox.add_child(col)
	vbox.add_child(hbox)
	_example_select_dialog.add_child(vbox)
	add_child(_example_select_dialog)

func _on_example_button_pressed(example_id: int) -> void:
	_example_select_dialog.hide()
	log_message("Loading Example Level: " + str(example_id))
	Global.MBEX.REMC2EditorExampleLevel(example_id)
	_on_map_type_state_changed_graphics_typeInt(Global.editorLevel["map_type"])

func _on_file_id_pressed(id: int) -> void:
	_ensure_file_dialogs()
	match id:
		MENU_FILE_EXPORT_CSV:
			Global.MBEX.REMC2EditorExportToCSV();
		MENU_FILE_QLOAD_LEVEL:
			var result = Global.MBEX.REMC2EditorLoadLevel("");
			if(!result):
				log_message("Can not Load this level file user://user-levels/editorLevel.mc2")
			else:
				log_message("Level loaded: " + "user://user-levels/quickSaved.mc2")
		MENU_FILE_QSAVE_LEVEL:
			Global.MBEX.REMC2EditorSaveLevel("");
			log_message("Level saved: " + "user://user-levels/quickSaved.mc2")
		MENU_FILE_LOAD_LEVEL:
			_load_dialog.popup_centered()
		MENU_FILE_SAVE_LEVEL:
			_save_dialog.popup_centered()
		MENU_FILE_RUN_LEVEL:
			Global.MBEX.REMC2EditorSaveLevel("user://user-levels/editorLevel.mc2")
			_runGame()
		MENU_FILE_CLEAN_LEVEL:
			Global.MBEX.REMC2EditorCleanLevel()
			_on_map_type_state_changed_graphics_typeInt(Global.editorLevel["map_type"])
		MENU_FILE_EXAMPLE_LEVEL:
			_ensure_example_select_dialog()
			_example_select_dialog.popup_centered()
		MENU_FILE_GAME_LEVEL:
			_ensure_level_select_dialog()
			_level_select_dialog.popup_centered()

const MENU_VIEW_TOGGLEROOF = 0
func _on_view_id_pressed(id: int) -> void:
	match id:
		MENU_VIEW_TOGGLEROOF:
			var popup = $UI/Control/MenuBar/View
			var idx = popup.get_item_index(MENU_VIEW_TOGGLEROOF)
			popup.set_item_checked(idx, !popup.is_item_checked(idx))
			var is_checked = popup.is_item_checked(idx)
			roof_show = is_checked
			SetRoofByVar()

func SetRoofByVar():
	if(!Main_TerrainsMB):
		return
	if(roof_show):
		Main_TerrainsMB.mesh_instance_top.show()
	else:
		Main_TerrainsMB.mesh_instance_top.hide()

func _runGame():
	var level_path = "user://user-levels/editorLevel.mc2"
	var my_args = ["--custom_level", level_path]
	OS.create_instance(my_args)
	get_tree().quit()

func UpdatePositionLabel():
	var x = Main_Camera.position.x
	var y = Main_Camera.position.z
	Position_Label.text = "Position: %06.2f x %06.2f" % [x, y]

var EntityFilter_On:bool=false

var FirstSelectedToEdit:bool=true

func _on_entity_spinbox_value_changed(_value = null) -> void:
	if _filling_entity_details:
		return
	_save_current_entity_changes(false)
	
func _save_current_entity_changes(commit_spinboxes = true) -> void:
	if current_entity_index <= 0 or current_entity_index >= pool_size:
		return
	if _filling_entity_details:
		return
	if commit_spinboxes:
		_force_commit_all_spinboxes(Entity_Edit)
	var idx = current_entity_index
	Global.editorLevel["entities"][idx]["axis_x"]   = Entity_Edit.get_node_or_null("POS/SpinBox").value as int
	Global.editorLevel["entities"][idx]["axis_y"]   = Entity_Edit.get_node_or_null("POS/SpinBox2").value as int
	Global.editorLevel["entities"][idx]["type"]      = Entity_Edit.get_node_or_null("type_0x30311/SpinBox").value as int
	Global.editorLevel["entities"][idx]["subtype"]   = Entity_Edit.get_node_or_null("subtype_0x30311/SpinBox").value as int
	Global.editorLevel["entities"][idx]["dis_id"]    = Entity_Edit.get_node_or_null("DisId/SpinBox").value as int
	Global.editorLevel["entities"][idx]["word10"]    = Entity_Edit.get_node_or_null("word_10/SpinBox").value as int
	Global.editorLevel["entities"][idx]["stage_tag"] = Entity_Edit.get_node_or_null("stageTag_12/SpinBox").value as int
	Global.editorLevel["entities"][idx]["par1"]      = Entity_Edit.get_node_or_null("par1_14/SpinBox").value as int
	Global.editorLevel["entities"][idx]["par2"]      = Entity_Edit.get_node_or_null("par2_16/SpinBox").value as int
	Global.editorLevel["entities"][idx]["par3"]      = Entity_Edit.get_node_or_null("par3_18/SpinBox").value as int
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)
	
func _force_commit_all_spinboxes(panel: Control = null) -> void:
	if panel == null:
		panel = Entity_Edit
	var spinboxes: Array = []
	if panel == Entity_Edit:
		spinboxes = [
			panel.get_node_or_null("POS/SpinBox"),
			panel.get_node_or_null("POS/SpinBox2"),
			panel.get_node_or_null("type_0x30311/SpinBox"),
			panel.get_node_or_null("subtype_0x30311/SpinBox"),
			panel.get_node_or_null("DisId/SpinBox"),
			panel.get_node_or_null("word_10/SpinBox"),
			panel.get_node_or_null("stageTag_12/SpinBox"),
			panel.get_node_or_null("par1_14/SpinBox"),
			panel.get_node_or_null("par2_16/SpinBox"),
			panel.get_node_or_null("par3_18/SpinBox"),
		]
	elif panel == Stages_Edit:
		spinboxes = [
			panel.get_node_or_null("StageIndex/SpinBox"),
			panel.get_node_or_null("StageStage/SpinBox"),
			panel.get_node_or_null("StageX1/SpinBox"),
			panel.get_node_or_null("StageY1/SpinBox"),
		]
	elif panel == StagesVars_Edit:
		spinboxes = [
			panel.get_node_or_null("StageIndex/SpinBox"),
			panel.get_node_or_null("StageStage/SpinBox"),
			panel.get_node_or_null("StageX1/SpinBox"),
			panel.get_node_or_null("StageY1/SpinBox"),
			panel.get_node_or_null("StageX2/SpinBox"),
			panel.get_node_or_null("StageY2/SpinBox"),
		]
	elif panel == Wizards_Edit:
		spinboxes = [
			panel.get_node_or_null("IDX/SpinBox"),
			panel.get_node_or_null("Aggression/SpinBox"),
			panel.get_node_or_null("Reflexes/SpinBox"),
			panel.get_node_or_null("Perception/SpinBox"),
			panel.get_node_or_null("Life/SpinBox"),
			panel.get_node_or_null("Activated/SpinBox"),
		]
	for sb in spinboxes:
		if sb and is_instance_valid(sb):
			var line_edit = sb.get_line_edit()
			if line_edit and line_edit.text.strip_edges() != "":
				sb.value = line_edit.text.to_int()

# ==================== WIZARDS (PLAYERS) ====================
func _save_current_wizards_changes(commit_spinboxes = true) -> void:
	if _filling_wizard_details:
		return
	if commit_spinboxes:
		_force_commit_all_spinboxes(Wizards_Edit)
	var idx = Wizards_Edit.get_node_or_null("IDX/SpinBox").value as int
	if idx < 0 or idx > 7:
		return
	Global.editorLevel["wizards"][idx]["aggression"]   = Wizards_Edit.get_node_or_null("Aggression/SpinBox").value as int
	Global.editorLevel["wizards"][idx]["reflexes"]     = Wizards_Edit.get_node_or_null("Reflexes/SpinBox").value as int
	Global.editorLevel["wizards"][idx]["perception"]   = Wizards_Edit.get_node_or_null("Perception/SpinBox").value as int
	Global.editorLevel["wizards"][idx]["life"]         = Wizards_Edit.get_node_or_null("Life/SpinBox").value as int
	Global.editorLevel["players"][idx]                 = Wizards_Edit.get_node_or_null("Activated/SpinBox").value as int
	var starting_container = Wizards_Edit.get_node_or_null("StartingSpells")
	if starting_container:
		var spells_data = []
		for panel in starting_container.get_children():
			var button = panel.get_child(0) if panel.get_child_count() > 0 else null
			if button and button is TextureButton:
				spells_data.append(button.button_pressed)
		Global.editorLevel["wizards"][idx]["starting_spells"] = spells_data
	var available_container = Wizards_Edit.get_node_or_null("AvailableSpells")
	if available_container:
		var spells_data = []
		for panel in available_container.get_children():
			var button = panel.get_child(0) if panel.get_child_count() > 0 else null
			if button and button is TextureButton:
				spells_data.append(button.button_pressed)
		Global.editorLevel["wizards"][idx]["byte_array"] = spells_data
	var blocked_container = Wizards_Edit.get_node_or_null("BlockedSpells")
	if blocked_container:
		var spells_data = []
		for panel in blocked_container.get_children():
			var button = panel.get_child(0) if panel.get_child_count() > 0 else null
			if button and button is TextureButton:
				spells_data.append(button.button_pressed)
		Global.editorLevel["wizards"][idx]["blocked_spells"] = spells_data
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)

func _on_wizard_spinbox_value_changed(_value = null) -> void:
	if _filling_wizard_details:
		return
	_save_current_wizards_changes(false)

var _filling_stages_details := false
var _filling_stagesvars_details := false

func _save_current_stages_changes(commit_spinboxes = true) -> void:
	if _filling_stages_details:
		return
	if commit_spinboxes:
		_force_commit_all_spinboxes(Stages_Edit)
	var idx = Stages_Edit.get_node_or_null("IDX/SpinBox").value as int
	if idx < 0 or idx >= Global.editorLevel["stages"].size():
		return
	Global.editorLevel["stages"][idx]["index"]  = Stages_Edit.get_node_or_null("StageIndex/SpinBox").value as int
	Global.editorLevel["stages"][idx]["stage"]  = Stages_Edit.get_node_or_null("StageStage/SpinBox").value as int
	Global.editorLevel["stages"][idx]["axis_x"] = Stages_Edit.get_node_or_null("StageX1/SpinBox").value as int
	Global.editorLevel["stages"][idx]["axis_y"] = Stages_Edit.get_node_or_null("StageY1/SpinBox").value as int
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)

func _on_stages_spinbox_value_changed(_value = null) -> void:
	if _filling_stages_details:
		return
	_save_current_stages_changes(false)

func _save_current_stagesvars_changes(commit_spinboxes = true) -> void:
	if _filling_stagesvars_details:
		return
	if commit_spinboxes:
		_force_commit_all_spinboxes(StagesVars_Edit)
	var idx = StagesVars_Edit.get_node_or_null("IDX/SpinBox").value as int
	if idx < 0 or idx >= Global.editorLevel["stage_vars"].size():
		return
	Global.editorLevel["stage_vars"][idx]["index"]         = StagesVars_Edit.get_node_or_null("StageIndex/SpinBox").value as int
	Global.editorLevel["stage_vars"][idx]["stage"]         = StagesVars_Edit.get_node_or_null("StageStage/SpinBox").value as int
	Global.editorLevel["stage_vars"][idx]["union_axis_2d_x"] = StagesVars_Edit.get_node_or_null("StageX1/SpinBox").value as int
	Global.editorLevel["stage_vars"][idx]["union_axis_2d_y"] = StagesVars_Edit.get_node_or_null("StageY1/SpinBox").value as int
	Global.editorLevel["stage_vars"][idx]["union_dword_axis_x"] = StagesVars_Edit.get_node_or_null("StageX2/SpinBox").value as int
	Global.editorLevel["stage_vars"][idx]["union_dword_axis_y"] = StagesVars_Edit.get_node_or_null("StageY2/SpinBox").value as int
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)
	
func _on_stagesvars_spinbox_value_changed(_value = null) -> void:
	if _filling_stagesvars_details:
		return
	_save_current_stagesvars_changes(false)

func _lock_spinbox_to_integers(sb: SpinBox) -> void:
	if not sb:
		return
	var le = sb.get_line_edit()
	if not le:
		return
	if not le.text_changed.is_connected(_on_spinbox_text_filter.bind(le)):
		le.text_changed.connect(_on_spinbox_text_filter.bind(le))

func _on_spinbox_text_filter(new_text: String, le: LineEdit) -> void:
	var filtered := ""
	for i in range(new_text.length()):
		var c := new_text[i]
		if c >= "0" and c <= "9":
			filtered += c
		elif c == "-" and i == 0:
			filtered += c
	if filtered != new_text:
		le.text = filtered
		le.caret_column = filtered.length()

func _connect_stage_spinbox(node) -> void:
	if not node:
		return
	_lock_spinbox_to_integers(node)
	if node and not node.value_changed.is_connected(_on_stages_spinbox_value_changed):
		node.value_changed.connect(_on_stages_spinbox_value_changed)
	if not node.focus_exited.is_connected(_on_stages_spinbox_value_changed):
		node.focus_exited.connect(_on_stages_spinbox_value_changed)

func _connect_stagesvar_spinbox(node) -> void:
	if not node:
		return
	_lock_spinbox_to_integers(node)
	if node and not node.value_changed.is_connected(_on_stagesvars_spinbox_value_changed):
		node.value_changed.connect(_on_stagesvars_spinbox_value_changed)
	if not node.focus_exited.is_connected(_on_stagesvars_spinbox_value_changed):
		node.focus_exited.connect(_on_stagesvars_spinbox_value_changed)

func _connect_stages_spinboxes() -> void:
	_connect_stage_spinbox(Stages_Edit.get_node_or_null("StageIndex/SpinBox"))
	_connect_stage_spinbox(Stages_Edit.get_node_or_null("StageStage/SpinBox"))
	_connect_stage_spinbox(Stages_Edit.get_node_or_null("StageX1/SpinBox"))
	_connect_stage_spinbox(Stages_Edit.get_node_or_null("StageY1/SpinBox"))

func _connect_stagesvars_spinboxes() -> void:
	_connect_stagesvar_spinbox(StagesVars_Edit.get_node_or_null("StageIndex/SpinBox"))
	_connect_stagesvar_spinbox(StagesVars_Edit.get_node_or_null("StageStage/SpinBox"))
	_connect_stagesvar_spinbox(StagesVars_Edit.get_node_or_null("StageX1/SpinBox"))
	_connect_stagesvar_spinbox(StagesVars_Edit.get_node_or_null("StageY1/SpinBox"))
	_connect_stagesvar_spinbox(StagesVars_Edit.get_node_or_null("StageX2/SpinBox"))
	_connect_stagesvar_spinbox(StagesVars_Edit.get_node_or_null("StageY2/SpinBox"))

func _connect_entity_spinbox(node) -> void:
	if not node:
		return
	_lock_spinbox_to_integers(node)
	if !node.value_changed.is_connected(_on_entity_spinbox_value_changed):
		node.value_changed.connect(_on_entity_spinbox_value_changed)
	if not node.focus_exited.is_connected(_on_entity_spinbox_value_changed):
		node.focus_exited.connect(_on_entity_spinbox_value_changed)

func _connect_entity_spinboxes() -> void:
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("POS/SpinBox"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("POS/SpinBox2"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("type_0x30311/SpinBox"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("subtype_0x30311/SpinBox"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("DisId/SpinBox"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("word_10/SpinBox"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("stageTag_12/SpinBox"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("par1_14/SpinBox"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("par2_16/SpinBox"))
	_connect_entity_spinbox(Entity_Edit.get_node_or_null("par3_18/SpinBox"))

func _connect_wizard_spinbox(node) -> void:
	if not node:
		return
	_lock_spinbox_to_integers(node)
	if !node.value_changed.is_connected(_on_wizard_spinbox_value_changed):
		node.value_changed.connect(_on_wizard_spinbox_value_changed)
	if not node.focus_exited.is_connected(_on_wizard_spinbox_value_changed):
		node.focus_exited.connect(_on_wizard_spinbox_value_changed)

func _connect_wizard_spells(node) -> void:
	if not node:
		return
	for panel in node.get_children():
		var button = panel.get_child(0) if panel.get_child_count() > 0 else null
		if button and button is TextureButton:
			if !button.toggled.is_connected(_on_wizard_spinbox_value_changed):
				button.toggled.connect(_on_wizard_spinbox_value_changed)

func _connect_wizards_spinboxes() -> void:
	_connect_wizard_spinbox(Wizards_Edit.get_node_or_null("Aggression/SpinBox"))
	_connect_wizard_spinbox(Wizards_Edit.get_node_or_null("Reflexes/SpinBox"))
	_connect_wizard_spinbox(Wizards_Edit.get_node_or_null("Perception/SpinBox"))
	_connect_wizard_spinbox(Wizards_Edit.get_node_or_null("Life/SpinBox"))
	_connect_wizard_spinbox(Wizards_Edit.get_node_or_null("Activated/SpinBox"))
	_connect_wizard_spells(Wizards_Edit.get_node_or_null("StartingSpells"))
	_connect_wizard_spells(Wizards_Edit.get_node_or_null("AvailableSpells"))
	_connect_wizard_spells(Wizards_Edit.get_node_or_null("BlockedSpells"))

func _on_map_type_state_changed_graphics_typeInt(state_index: int) -> void:
	if(state_index==0):
		_on_map_type_state_changed_graphics_type("Day")
	if(state_index==1):
		_on_map_type_state_changed_graphics_type("Night")
	if(state_index==2):
		_on_map_type_state_changed_graphics_type("Cave")

func _on_map_type_state_changed_graphics_type(state_name: String) -> void:
	if(!Main_TerrainsMB):
		return
	if(state_name=="Cave"):
		updateRoof=true
	else:
		updateRoof=false
	Global.setLevelType(state_name)
	Global.MBEX.REMC2SetLevelType(state_name)
	Main_DecodeLevel.gameInit(false)
	Main_TerrainsMB.updateMeshes(false)
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	RenderEditorEntites()
	SetRoofByVar()

func _on_filter_id_pressed(id: int) -> void:
	_ensure_file_dialogs()
	match id:
		MENU_FILTER_SELECTRAY:
			var popup = $UI/Control/MenuBar/Filter
			var idx = popup.get_item_index(MENU_FILTER_SELECTRAY)
			popup.set_item_checked(idx, !popup.is_item_checked(idx))
			var is_checked = popup.is_item_checked(idx)
			Ray_Cylinder.visible = is_checked
		MENU_FILTER_SELECTFILTER:
			var popup = $UI/Control/MenuBar/Filter
			var idx = popup.get_item_index(MENU_FILTER_SELECTFILTER)
			popup.set_item_checked(idx, !popup.is_item_checked(idx))
			var is_checked = popup.is_item_checked(idx)
			EntityFilter_On = is_checked
		MENU_FILTER_SHOWFILTER:
			var popup = $UI/Control/MenuBar/Filter
			var idx = popup.get_item_index(MENU_FILTER_SHOWFILTER)
			popup.set_item_checked(idx, !popup.is_item_checked(idx))
			var is_checked = popup.is_item_checked(idx)
			Ray_Cylinder.visible = is_checked
		MENU_FILTER_SELECTTOEDIT:
			var popup = $UI/Control/MenuBar/Filter
			var idx = popup.get_item_index(MENU_FILTER_SELECTTOEDIT)
			popup.set_item_checked(idx, !popup.is_item_checked(idx))
			var is_checked = popup.is_item_checked(idx)
			FirstSelectedToEdit = is_checked

func _on_entity_set_button_down() -> void:
	if current_edited_entity <= 0 or current_edited_entity >= pool_size:
		return
	var pos = Main_Camera.position
	Global.editorLevel["entities"][current_edited_entity]["axis_x"] = int(pos.x)
	Global.editorLevel["entities"][current_edited_entity]["axis_y"] = int(pos.z)
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)
	EditorStep()
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	#_save_current_entity_changes(false)
	RenderEditorEntites()
	fillEntityDetails(current_edited_entity)

func _on_entity_move_button_down() -> void:
	if current_edited_entity <= 0 or current_edited_entity >= pool_size:
		return
	for node in get_tree().get_nodes_in_group("entities"):
		if node.get_meta("index") == current_edited_entity:
			Main_Camera.position = node.position
