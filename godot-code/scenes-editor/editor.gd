extends Node3D

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

@onready var Console: RichTextLabel = $UI/Console/RichTextLabel

@onready var Terrain_Edit = $UI/TerrainEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer
@onready var Entity_Edit: Control = $UI/EntityEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer
@onready var Wizards_Edit: Control = $UI/WizardsEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer
@onready var Stages_Edit: Control = $UI/StagesEdit/PreContainer/PanelContainer/MarginContainer/VBoxContainer

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
	Terrain_Edit.get_node("rkSte")
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
var is_ui_visible = false

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
	for i in range(selectors.size()):
		var selector = selectors[i]
		selector.current_value = Global.editorLevel[selectors[i].name]
		selector.get_node_or_null("SpinBox").value = Global.editorLevel[selectors[i].name]
		if not selector.get_node_or_null("SpinBox").value_changed.is_connected(_on_h_box_container_value_changed):
			selector.get_node_or_null("SpinBox").value_changed.connect(_on_h_box_container_value_changed.bind(i))
	
	for i in range(TEselector2.size()):
		var selector = TEselector2[i]
		selector.get_node_or_null("SpinBox").value = Global.editorLevel[TEselector2[i].name]
		if not selector.get_node_or_null("SpinBox").value_changed.is_connected(_on_h_box_container_value_changed2):
			selector.get_node_or_null("SpinBox").value_changed.connect(_on_h_box_container_value_changed2.bind(i))
	
	
	
	#SetPlayerValues(Global.MBEX.REMC2EditorGetTerrainPlayers())
	#SetStagesValues(Global.MBEX.REMC2EditorGetTerrainStages())

	toggle_editor_control_styleSt(true)
	$UI.visible=true
	_preload_library(library, library_scenes)
	
	RenderEditorEntites()
	
	Tree_View.item_selected.connect(_on_tree_item_selected)
	
	get_window().focus_entered.connect(_on_window_focus_entered)
	get_window().focus_exited.connect(_on_window_focus_exited)
	
	Wizards_Edit.display_player_data(0)
	Stages_Edit.display_stages_data(0)
	editor_runned=true

func _on_window_focus_entered() -> void:
	toggle_editor_control_styleSt(is_ui_visible)
	log_message("Aplikace opět aktivní - myš nastavena.")

func _on_window_focus_exited() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	log_message("Aplikace ztratila focus - myš uvolněna.")
	
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
	
func _input(event: InputEvent) -> void:
	# M pressed
	if (event is InputEventKey and event.keycode == KEY_SPACE and event.pressed):
		toggle_editor_control_style()
	if (event is InputEventKey and event.keycode == KEY_H and event.pressed):
		$UI.visible = !$UI.visible
	if (event is InputEventKey and event.keycode == KEY_X and event.pressed):
		delete_selected_entities()
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
		Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED)
	else:
		Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
		#var center = get_viewport().get_visible_rect().size / 2.0
		#get_viewport().warp_mouse(center)

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
	all_sections.append({ "title": "Entities", "items": entity_items })
	
	# ── Next ────────────────────────────────────────────────────
	var next_items = [] #UPDATE
	next_items.append({ #UPDATE
		"name": "next_360D1", #UPDATE
		"value": str(Global.editorLevel.get("next_360D1", 0)), #UPDATE
		"id": 0 #UPDATE
	}) #UPDATE
	all_sections.append({ "title": "Next", "items": next_items })

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
			"name": "SV%d  Idx:%d  Stg:%d  X:%d  Y:%d" % [i, v.get("index",0), v.get("stage",0), v.get("union_axis_2d_x",0), v.get("union_axis_2d_y",0)], #UPDATE
			"value": str(i), #UPDATE
			"id": i #UPDATE
		}) #UPDATE
	all_sections.append({ "title": "StagesVars", "items": stagesvars_items })
	
	#uint8_t next_0x360D1;
	#Type_WizardMapSettings_0x360D2 WizardMapSettings_0x360D2[8];//lenght 110  /spells?
	#type_str_0x36442 stages_0x36442[8];//stages(checkpoints)
	#type_str_0x3647Ac StageVars_0x3647A[11];//8x11
	#var next_items = []
	#all_sections.append({ "title": "Next", "items": next_items })
	#var spells_items = []	
	#spells_items.append({
		#"name": "Speels",
		#"value": "X",
		#"id": 0
	#})	
	#all_sections.append({ "title": "Spells", "items": spells_items })
	#var stages_items = []
	#stages_items.append({
		#"name": "Stages",
		#"value": "X",
		#"id": 0
	#})	
	#all_sections.append({ "title": "Stages", "items": stages_items })
	#var stagesvars_items = []
	#stagesvars_items.append({
		#"name": "StagesVars",
		#"value": "X",
		#"id": 0
	#})	
	#all_sections.append({ "title": "StagesVars", "items": stagesvars_items })

	Tree_View.update_tree_view(all_sections)
	
		# Podbarvi vybrané entity v tree
	var selected_indices = {}
	for node in get_tree().get_nodes_in_group("selected_entities"):
		if node.has_meta("index"):
			selected_indices[node.get_meta("index")] = true

	var root = Tree_View.get_root()
	if root == null:
		return
	var section = root.get_first_child()
	while section:
		if section.get_text(0) == "Entities":
			var item = section.get_first_child()
			while item:
				var idx = item.get_metadata(0)
				if idx in selected_indices:
					item.set_custom_bg_color(0, Color(1.0, 1.0, 0.0, 0.4))
					item.set_custom_bg_color(1, Color(1.0, 1.0, 0.0, 0.4))
				else:
					item.clear_custom_bg_color(0)
					item.clear_custom_bg_color(1)
				item = item.get_next()
		section = section.get_next()

func _process(delta: float) -> void:
	if editor_runned:
		EditorStep()
	update_tree()
	Global.MBEX.REMC2EditorTimedSaveState(1.0)
	UpdatePositionLabel()
	select_entities_by_filter()
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

func EditorStep():
	Global.MBEX.REMC2EditorLoop()
	var isCave:bool = false
	Global.MBEX.renew_terrain(isCave)

var pool_size = 1020

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
			unmark_as_selected(node)
			if dist_sq < min_dist_sq:
				min_dist_sq = dist_sq
				closest_node = node
			if dist_sq <= radius_squared:
				mark_as_selected(node)
				found_any_in_radius = true
		if not found_any_in_radius and closest_node != null:
			mark_as_selected(closest_node)

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
			mark_as_selected(node)
		else:
			unmark_as_selected(node)

func mark_as_selected(node: Node3D):
	if node.is_in_group("selected_entities"):
		return
	node.add_to_group("selected_entities")	
	var mesh = node.get_node_or_null("MeshInstance3D")
	if mesh:
		var mat = mesh.get_surface_override_material(0)
		if not mat:
			mat = mesh.mesh.surface_get_material(0).duplicate()
			mesh.set_surface_override_material(0, mat)
		mat.albedo_color = Color(1.0, 1.0, 0.0)

func unmark_as_selected(node: Node3D):
	if not node.is_in_group("selected_entities"): return
	node.remove_from_group("selected_entities")	
	var mesh = node.get_node_or_null("MeshInstance3D")
	if mesh:
		var mat = mesh.get_surface_override_material(0)
		if mat:
			mat.albedo_color = Color(1, 1, 1) 

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
	RenderEditorEntites()

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
						"Pos: %d,%d,%d\n" +
						"DisId: %d, " +
						"W10: %d, " +
						"Stage: %d\n" +
						"Par1: %d, " +
						"Par2: %d, " +
						"Par3: %d"
					) % [
						i,
						type_0x30311, subtype_0x30311,
						pos.x,pos.y,pos.z,
						DisId, word_10, stageTag_12, 
						par1_14, par2_16, par3_18
					]
			
			current_node.set_meta("index",int(i))
			
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
	#AddDisIdArrows()
	AddDisId2Arrows()
	show_hide_entites()

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

func _on_terrain_type_state_changed_graphics_type(state_name: String) -> void:
	if(!Main_TerrainsMB):
		return
	Global.setLevelType(state_name)
	Global.MBEX.REMC2SetLevelType(state_name)
	Main_DecodeLevel.gameInit(false)
	Main_TerrainsMB.updateMeshes(false)
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	RenderEditorEntites()

func _on_h_box_container_value_changed(new_value: int,terrainVarIndex: int) -> void:
	Global.editorLevel[selectors[terrainVarIndex].name] = new_value
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)
	EditorStep()
	Global.editorLevel = Global.MBEX.REMC2EditorGetLevelData()
	RenderEditorEntites()

func _on_h_box_container_value_changed2(new_value: int,terrainVarIndex: int) -> void:
	Global.editorLevel[TEselector2[terrainVarIndex].name] = new_value
	Global.MBEX.REMC2EditorSetLevelData(Global.editorLevel)

func _on_undo_button_down() -> void:
	Global.MBEX.REMC2EditorUndo()
	log_message("UNDO")

func _on_redo_button_down() -> void:
	Global.MBEX.REMC2EditorRedo()
	log_message("REDO")

func SaveState() -> void:
	Global.MBEX.REMC2EditorSaveState()

func log_message(text: String, color: String = "white"):
	var formatted_text = "[color=" + color + "]" + text + "[/color]\n"
	Console.append_text(formatted_text)
	var scrollbar = Console.get_v_scroll_bar()
	scrollbar.value = scrollbar.max_value

func fillEntityDetails(index:int):
	var entities = get_tree().get_nodes_in_group("entities")
	var finded_node = null
	for node in entities:
		if node.has_meta("index") and node.get_meta("index") == index:
			finded_node=node
	if finded_node:
		Entity_Edit.get_node_or_null("IDX/SpinBox").value = finded_node.get_meta("index")
		Entity_Edit.get_node_or_null("type_0x30311/SpinBox").value = finded_node.get_meta("type_0x30311")
		Entity_Edit.get_node_or_null("subtype_0x30311/SpinBox").value = finded_node.get_meta("subtype_0x30311")
		Entity_Edit.get_node_or_null("DisId/SpinBox").value = finded_node.get_meta("DisId")
		Entity_Edit.get_node_or_null("word_10/SpinBox").value = finded_node.get_meta("word_10")
		Entity_Edit.get_node_or_null("stageTag_12/SpinBox").value = finded_node.get_meta("stageTag_12")
		Entity_Edit.get_node_or_null("par1_14/SpinBox").value = finded_node.get_meta("par1_14")
		Entity_Edit.get_node_or_null("par2_16/SpinBox").value = finded_node.get_meta("par2_16")
		Entity_Edit.get_node_or_null("par3_18/SpinBox").value = finded_node.get_meta("par3_18")
		
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
	var item_value = selected.get_text(1)   # hodnota jako string
	var item_index: int = -1
	if selected.get_metadata(0) != null:
		item_index = selected.get_metadata(0)
	match section_title:
		"Terrain":
			_on_tree_terrain_selected(item_index, item_value.to_int())
		"Entities":
			_on_tree_entity_selected(item_index, item_value)
		"Spells":
			_on_tree_terrain_spells(item_index, item_value)
		"Stages":
			_on_tree_terrain_stages(item_index, item_value)
		"StagesVars":
			_on_tree_terrain_stagesVars(item_index, item_value)

func _on_tree_terrain_selected(index: int, value: int) -> void:
	log_message("Terrain selected: index=%d value=%d" % [index, value])
	Terrain_Edit_Panel.show()
	Entity_Edit_Panel.hide()
	Wizards_Edit_Panel.hide()
	Stages_Edit_Panel.hide()

func _on_tree_entity_selected(index: int, value: String) -> void:
	log_message("Entity selected: index=%d value=%s" % [index, value])
	Terrain_Edit_Panel.hide()
	Entity_Edit_Panel.show()
	Wizards_Edit_Panel.hide()
	Stages_Edit_Panel.hide()
	fillEntityDetails(index)
	
func _on_tree_terrain_spells(index: int, value: String):
	Wizards_Edit_Panel.show()
	Terrain_Edit_Panel.hide()
	Entity_Edit_Panel.hide()
	Stages_Edit_Panel.hide()

func _on_tree_terrain_stages(index: int, value: String):
	Terrain_Edit_Panel.hide()
	Entity_Edit_Panel.hide()
	Wizards_Edit_Panel.show()
	Stages_Edit_Panel.hide()
 
func _on_tree_terrain_stagesVars(index: int, value: String):
	Terrain_Edit_Panel.hide()
	Entity_Edit_Panel.hide()
	Wizards_Edit_Panel.hide()
	Stages_Edit_Panel.show()

const ID_EXPORT_CSV = 0
const ID_LOAD_LEVEL = 1
const ID_SAVE_LEVEL = 2
const ID_RUN_LEVEL = 3
func _on_file_id_pressed(id: int) -> void:
	match id:
		ID_EXPORT_CSV:
			Global.MBEX.REMC2EditorExportToCSV();
		ID_LOAD_LEVEL:
			Global.MBEX.REMC2EditorLoadLevel();
		ID_SAVE_LEVEL:
			Global.MBEX.REMC2EditorSaveLevel();
		ID_RUN_LEVEL:
			Global.MBEX.REMC2EditorSaveLevel();
			_runGame()

func _runGame():
	var level_path = "user://user-levels/level0.mc2"
	var my_args = ["--custom_level", level_path]
	OS.create_instance(my_args)
	get_tree().quit()

func UpdatePositionLabel():
	var x = Main_Camera.position.x
	var y = Main_Camera.position.z
	Position_Label.text = "Position: %06.2f x %06.2f" % [x, y]

func _on_selector_toggled(toggled_on: bool) -> void:
	Ray_Cylinder.visible = toggled_on
	if(Ray_Cylinder.visible):
		if(EntityFilter_On):
			$UI/Control/Filter.button_pressed=false

var EntityFilter_On:bool=false
func _on_filter_toggled(toggled_on: bool) -> void:
	EntityFilter_On = toggled_on
	if(EntityFilter_On):
		if(Ray_Cylinder.visible):
			$UI/Control/Selector.button_pressed=false
	else:
		for node in get_tree().get_nodes_in_group("entities"):
			unmark_as_selected(node)


func _on_filter_show_toggled(toggled_on: bool) -> void:
	EntityFilter.visible = toggled_on
