@tool
extends Camera3D

## ─────────────────────────────────────────────
##  CameraFrustumTester.gd
##  Přidej tento skript na Camera3D node.
##  Všechny parametry jsou viditelné v Inspectoru.
##  Skript funguje i v editoru (@tool).
## ─────────────────────────────────────────────

@export_group("Stav přechodu")
## 0 = herní pohled (A), 1 = mapový pohled (B)
@export_range(0.0, 1.0, 0.1) var camera_state: float = 0.0:
	set(v): camera_state = v; _apply_frustum()

@export_range(0.1, 0.5, 0.1) var default_steps_count: float = 0.1

@export_group("Parametry A (hra)")
@export_range(1.0, 170.0, 0.5) var fov_a: float = 75.0:
	set(v): fov_a = v; _apply_frustum()
@export_range(0.001, 10.0, 0.001) var near_a: float = 0.25:
	set(v): near_a = v; _apply_frustum()
@export_range(100.0, 65536.0, 100.0) var far_a: float = 16384.0:
	set(v): far_a = v; _apply_frustum()
@export_range(-3.0, 3.0, 0.01) var left_a: float = -1.0:
	set(v): left_a = v; _apply_frustum()

@export_group("Parametry B (mapa)")
@export_range(1.0, 170.0, 0.5) var fov_b: float = 75.0:
	set(v): fov_b = v; _apply_frustum()
@export_range(0.001, 10.0, 0.001) var near_b: float = 0.25:
	set(v): near_b = v; _apply_frustum()
@export_range(100.0, 65536.0, 100.0) var far_b: float = 16384.0:
	set(v): far_b = v; _apply_frustum()
@export_range(-3.0, 3.0, 0.01) var left_b: float = 0.6:
	set(v): left_b = v; _apply_frustum()

@export_group("Debug výstup")
## Vypiš vypočtené hodnoty do Output panelu při každé změně
@export var log_values: bool = true
## Zvýrazni chybu ve z_far vzorci (FarA ignorováno)
@export var warn_zfar_bug: bool = true

# ── vypočtené hodnoty (read-only, viditelné v Inspectoru) ──
@export_group("Výsledky (read-only)")
@export var _result_fov: float = 0.0
@export var _result_z_near: float = 0.0
@export var _result_z_far: float = 0.0
@export var _result_size: float = 0.0
@export var _result_left_part: float = 0.0
@export var _result_desired_center_x: float = 0.0
@export var _result_offset_x: float = 0.0
@export var _result_frustum_offset: Vector2 = Vector2.ZERO


func _ready() -> void:
	_apply_frustum()


func _apply_frustum() -> void:
	var t := camera_state

	# ── interpolace ──
	var fov      := lerpf(fov_a,  fov_b,  t)
	var z_near   := lerpf(near_a, near_b, t)
	var z_far    := lerpf(far_a,  far_b,  t)          # SPRÁVNĚ: používá far_a i far_b
	var left_part := lerpf(left_a, left_b, t)

	# ── BUG ve původním kódu ──
	var z_far_buggy := far_b * t + far_b * (1.0 - t)  # = vždy far_b !
	if warn_zfar_bug and absf(z_far_buggy - z_far) > 0.01:
		push_warning(
			"[CameraFrustumTester] BUG v z_far: původní vzorec dává %.1f místo %.1f " \
			% [z_far_buggy, z_far] +
			"— obě strany používají far_b, far_a je ignorováno!"
		)

	# ── výpočet frustumu ──
	var viewport_size := get_viewport().get_visible_rect().size if get_viewport() else Vector2(1920, 1080)
	var aspect        := viewport_size.x / viewport_size.y

	var size             := 2.0 * tan(deg_to_rad(fov / 2.0)) * z_near
	var desired_center_x := (left_part + 1.0) / 2.0 - 0.5
	var offset_x         := desired_center_x * (size * aspect)
	var frustum_offset   := Vector2(offset_x, 0.0)

	#var desired_center_x := -(left_part + (1.0 - left_part) / 2.0)
	#var size             := 2.0 * tan(deg_to_rad(fov / 2.0)) * z_near
	#var offset_x         := (desired_center_x - 0.5) * 2.0 * (size * 0.5 * aspect)
	#var frustum_offset   := Vector2(offset_x * (size * 0.5 * aspect), 0.0)

	# ── ulož výsledky do Inspectoru ──
	_result_fov            = snappedf(fov, 0.001)
	_result_z_near         = snappedf(z_near, 0.0001)
	_result_z_far          = snappedf(z_far, 0.1)
	_result_size           = snappedf(size, 0.0001)
	_result_left_part      = snappedf(left_part, 0.0001)
	_result_desired_center_x = snappedf(desired_center_x, 0.0001)
	_result_offset_x       = snappedf(offset_x, 0.0001)
	_result_frustum_offset = Vector2(snappedf(frustum_offset.x, 0.0001), 0.0)

	# ── aplikuj na kameru ──
	projection = Camera3D.PROJECTION_FRUSTUM
	set_frustum(size, frustum_offset, z_near, z_far)

	if log_values:
		print("──── CameraFrustumTester (state=%.2f) ────" % t)
		print("  fov:             %.4f°" % fov)
		print("  z_near:          %.4f"  % z_near)
		print("  z_far (správně): %.1f"  % z_far)
		if warn_zfar_bug:
			print("  z_far (bug):     %.1f  ← původní vzorec" % z_far_buggy)
		print("  aspect:          %.4f"  % aspect)
		print("  left_part:       %.4f"  % left_part)
		print("  desired_center_x:%.4f"  % desired_center_x)
		print("  size:            %.4f"  % size)
		print("  offset_x:        %.4f"  % offset_x)
		print("  frustum_offset:  %s"    % str(frustum_offset))
