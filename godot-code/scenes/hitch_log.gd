extends Node
# The log of the long frames, with the user argument --debug-hitches (added by Global): every frame
# THRESHOLD_MS over the frame limit goes to the debug log (--debug-log) or stderr with where its time went - the
# parts of MBrun (Global.prof), of the engine (getFrameProfile), the draw on the main thread, the
# pipelines compiled - and what happened in it (Global.prof_events: scenes waited for, entity nodes
# made during the game, slow sound calls). Every 10 s a line with the frame times.

const THRESHOLD_MS := 30.0
const PIPELINES := [RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_CANVAS,
	RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_MESH, RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_SURFACE,
	RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_DRAW, RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_SPECIALIZATION]

# the first node to process: when the process part of the frame starts
class First extends Node:
	var start_usec := 0
	func _ready():
		process_priority = -100000
	func _process(_delta):
		start_usec = Time.get_ticks_usec()

var _first: First
var _pre_draw := 0
var _post_draw := 0
var _pending := {} # the previous frame: its draw and its length are known only now
var _frame := 0
var _pipelines_last := 0
var _nodes_last := 0
var _summary_start := 0
var _summary_frames := 0
var _summary_max := 0.0
var _summary_hitches := 0
var _scene_name := ""

func _ready() -> void:
	process_priority = 100000 # after all other nodes
	process_mode = Node.PROCESS_MODE_ALWAYS
	_first = First.new()
	_first.process_mode = Node.PROCESS_MODE_ALWAYS
	get_parent().add_child.call_deferred(_first)
	RenderingServer.frame_pre_draw.connect(func(): _pre_draw = Time.get_ticks_usec())
	RenderingServer.frame_post_draw.connect(func(): _post_draw = Time.get_ticks_usec())
	_summary_start = Time.get_ticks_msec()
	_log("hitch log on: frames %d ms over the frame limit" % THRESHOLD_MS)

func _log(text: String) -> void:
	if MBDebug.enabled():
		MBDebug.log(text)
	else:
		printerr(text)

func _process(_delta: float) -> void:
	var now := Time.get_ticks_usec()
	var scene = get_tree().current_scene
	var scene_name: String = scene.name if scene else "-"
	if scene_name != _scene_name:
		_log("scene %s" % scene_name)
		_scene_name = scene_name
	var pipelines := 0
	for info in PIPELINES:
		pipelines += RenderingServer.get_rendering_info(info)
	var nodes := int(Performance.get_monitor(Performance.OBJECT_NODE_COUNT))
	var row := {
		"start": _first.start_usec,
		"process": (now - _first.start_usec) / 1000.0,
		"prof": Global.prof.duplicate(),
		"engine": Global.MBEX.getFrameProfile() if Global.MBEX else {},
		"events": Global.prof_events.duplicate(),
		"pipelines": pipelines - _pipelines_last,
		"nodes": nodes - _nodes_last,
	}
	Global.prof.clear()
	Global.prof_events.clear()
	_pipelines_last = pipelines
	_nodes_last = nodes
	if not _pending.is_empty():
		_report(_pending, (_first.start_usec - _pending["start"]) / 1000.0, (_post_draw - _pre_draw) / 1000.0)
	_pending = row
	_frame += 1

func _report(row: Dictionary, frame_ms: float, render_ms: float) -> void:
	_summary_frames += 1
	_summary_max = max(_summary_max, frame_ms)
	# over the frame limit by THRESHOLD_MS - the game runs at 24 or 30 fps, its frames are 33-42 ms
	var budget := 1000.0 / Engine.max_fps if Engine.max_fps > 0 else 0.0
	if frame_ms > budget + THRESHOLD_MS:
		_summary_hitches += 1
		_log("HITCH frame %d %.1f ms: process %.1f, draw %.1f | %s | engine %s | pipelines %d, nodes %+d | %s" % [
			_frame - 1, frame_ms, row["process"], render_ms, _top(row["prof"]), _top(row["engine"]),
			row["pipelines"], row["nodes"], "; ".join(row["events"]) if not row["events"].is_empty() else "-"])
	var now := Time.get_ticks_msec()
	if now - _summary_start >= 10000:
		_log("frames %d in %.1f s, longest %.1f ms, over %d ms: %d" % [_summary_frames,
			(now - _summary_start) / 1000.0, _summary_max, THRESHOLD_MS, _summary_hitches])
		_summary_start = now
		_summary_frames = 0
		_summary_max = 0.0
		_summary_hitches = 0

# the parts over 1 ms, longest first
func _top(parts: Dictionary) -> String:
	var keys := parts.keys().filter(func(k): return float(parts[k]) >= 1.0)
	keys.sort_custom(func(a, b): return float(parts[a]) > float(parts[b]))
	return ", ".join(keys.map(func(k): return "%s %.1f" % [k, float(parts[k])])) if not keys.is_empty() else "-"

func _notification(what: int) -> void:
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		_log("window close requested")
	elif what == NOTIFICATION_PREDELETE:
		_log("hitch log freed (the scene tree is being freed)")
