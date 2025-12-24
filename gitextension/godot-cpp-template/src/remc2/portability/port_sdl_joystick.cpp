
// support for gamepads and joysticks
//
// adds configurable game specific controls to be bound to axes, buttons and hats present on a controller
// see README_controllers for sample configurations
// this implementation basically simulates mouse moves and key presses
//
// tested with Logitech Attack 3, Logitech Extreme PRO 3D and XBOX Elite Series 2
//
// author: Petre Rodan, 2023

#include <cstdint>
#include <stdio.h>

//#include "SDL2/SDL.h"

#include "../engine/sub_main_mouse.h"
#include "../engine/read_config.h"
#include "../utilities/Maths.h"
#include "port_sdl_vga_mouse.h"
#include "port_sdl_joystick.h"

//SDL_Joystick *m_gameController = NULL;
//SDL_Haptic *m_haptic = NULL;

#define              JOY_MIN_X  0   ///< minimum bounds for mouse position value for x axis
#define              JOY_MIN_Y  0   ///< minimum bounds for mouse position value for y axis
#define            JOY_NAV_INC  4   ///< coefficient that defines how many pixels to skip while navigating a menu

#define       GP_FLIGHT_UPDATE  0x1 ///< bitflag set if the axes controlling flight are out of the stick dead zone
#define          GP_NAV_UPDATE  0x2 ///< bitflag set if the axes controlling navigation are out of the stick dead zone
#define          GP_MOV_UPDATE  0x4 ///< bitflag set if the axes controlling movement are out of the stick dead zone

#define GP_MAX_KEY_RELEASE_ANN  4   ///< maximum number of key release announcements

///< simulated key presses
// to be modified once fully customized 
// keyboard control is implemented
#define          GP_KEY_EMU_UP  0x5252
#define        GP_KEY_EMU_DOWN  0x5151
#define       GP_KEY_EMU_RIGHT  0x4f4f
#define        GP_KEY_EMU_LEFT  0x5050
#define     GP_KEY_EMU_MINIMAP  0x280d
#define         GP_KEY_EMU_ESC  0x291b
#define       GP_KEY_EMU_SPELL  0xe0e0
#define		  GP_KEY_EMU_PAUSE  0x1370
#define		  GP_KEY_EMU_SPACE  0x2C20

///< structure that defines the current gamepad state ad it's simulated output
struct gamepad_state {
	int32_t x;                      ///< currently simulated x mouse position
	int32_t y;                      ///< currently simulated y mouse position
	int32_t rest_x;                 ///< mouse x position to use when the stick is in the rest position (or dead zone)
	int32_t rest_y;                 ///< mouse y position to use when the stick is in the rest position (or dead zone)
	int32_t max_x;                  ///< maximum bounds for mouse position on the x axis based on current scene (sometimes we get garbage values here from upstream)
	int32_t max_y;                  ///< maximum bounds for mouse position on the y axis based on current scene (sometimes we get garbage values here from upstream)
	uint8_t dead_zone_announced;    ///< slow infinite spin mitigation when joystick is in the resting position while in the flying window
	uint8_t mov_key_announced;      ///< counter of consecutive setPress(false, KEY) requests 
	uint8_t initialized;            ///< gamepad was initialized and it's ready to be queried
	uint8_t scene_id;				///< current scene displayed by the recode. one of SCENE_PREAMBLE_MENU, SCENE_FLIGHT, SCENE_FLIGHT_MENU
	uint8_t nav_mode;               ///< true during menu navigation
	uint8_t last_trig_fire_R;       ///< detection of movement based on the right trigger button's axis value
	uint8_t last_trig_fire_L;       ///< detection of movement based on the left trigger button's axis value
};
typedef struct gamepad_state gamepad_state_t;

///< structure that defines the mouse pointer position
struct pointer_sys {
	int16_t x;
	int16_t y;
};
typedef struct pointer_sys pointer_sys_t;

///< hat position data
struct vec1d {
	int16_t x;                      ///< bitwise flags. SDL_HAT_UP | SDL_HAT_DOWN | SDL_HAT_LEFT | SDL_HAT_RIGHT or 0
	uint8_t x_conf;                 ///< bitwise flags. cound be 0 or a combination of GAMEPAD_AXIS_INVERTED | GAMEPAD_ITEM_ENABLED
};
typedef struct vec1d vec1d_t;

///< axis position data
struct vec2d {
	int16_t x;                      ///< x axis value [-32767..32768]
	int16_t y;                      ///< y axis value [-32767..32768]
	uint8_t x_conf;                 ///< bitwise flags. cound be 0 or a combination of GAMEPAD_AXIS_INVERTED | GAMEPAD_ITEM_ENABLED
	uint8_t y_conf;                 ///< bitwise flags. cound be 0 or a combination of GAMEPAD_AXIS_INVERTED | GAMEPAD_ITEM_ENABLED
};
typedef struct vec2d vec2d_t;

///< force-feedback subsystem state
struct haptic_state {
	uint8_t enabled;                ///< if subsystem is currently enabled
	uint8_t initialized;            ///< subsystem initialized and ready
	uint8_t rumble;                 ///< rumble is initialized
	uint8_t rumble_trig;            ///< rumble trigger is present
	uint32_t cap;                   ///< controller capabilities
	int quake;                      ///< quake effect identifier
};
typedef struct haptic_state haptic_state_t;

gamepad_state_t gps = {};
haptic_state_t hs = {};

int8_t haptic_load_effects(void);

/// \brief initialization of the SDL joystick subsystem
void gamepad_sdl_init(void)
{
}

/// \brief cleanup of the SDL joystick subsystem, to be used only on program exit
void gamepad_sdl_close(void)
{
}

/// \brief initialize gamepad maximal coordinate values, default operating mode, etc
/// \param gameResWidth maximum x value
/// \param gameResHeight maximum y value
void gamepad_init(const int gameResWidth, const int gameResHeight)
{
	gps.max_x = gameResWidth;
	gps.max_y = gameResHeight;
	joystick_set_env(gps.max_x >> 1, gps.max_y >> 1);
	set_scene(SCENE_PREAMBLE_MENU);
}

void AdjustStickCoords(vec2d_t* stick, std::vector<Maths::Zone>* zonesX, std::vector<Maths::Zone>* zonesY)
{
	if (stick->x >= 0)
	{
		stick->x = ((int16_t)Maths::CurveCoords(stick->x, stick->x, *zonesX));
	}
	else
	{
		stick->x = -((int16_t)Maths::CurveCoords(-stick->x, -stick->x, *zonesX));
	}

	if (stick->y >= 0)
	{
		stick->y = ((int16_t)Maths::CurveCoords(stick->y, stick->y, *zonesY));
	}
	else
	{
		stick->y = -((int16_t)Maths::CurveCoords(-stick->y, -stick->y, *zonesY));
	}
}

/// \brief flight support via conversion from stick coordinates to pointer coordinates
/// \param  stick input axis values
/// \param  point output simulated mouse pointer values
/// \return 0 if stick is in the dead zone or GP_FLIGHT_UPDATE otherwise
uint16_t gamepad_axis_flight_conv(vec2d_t *stick, pointer_sys_t *point)
{
	uint16_t ret = 0;
	AdjustStickCoords(stick, &gpc.axis_yaw_sensitivity, &gpc.axis_pitch_sensitivity);
	int16_t axis_yaw = stick->x;
	int16_t axis_pitch = stick->y;

	if ((axis_yaw < gpc.axis_yaw_dead_zone) && (axis_yaw > -gpc.axis_yaw_dead_zone)) {
		point->x = gps.rest_x;
	} else {
		// use two different linear interpolation equations since the
		// resting coordinate is not always the center of the display
		if (axis_yaw > 0) {
			point->x = (((gps.max_x - gps.rest_x) * axis_yaw) >> 15) + gps.rest_x;
		} else {
			point->x = ((gps.rest_x * axis_yaw) >> 15) + gps.rest_x;
		}
		ret = GP_FLIGHT_UPDATE;
	}

	if ((axis_pitch < gpc.axis_pitch_dead_zone) && (axis_pitch > -gpc.axis_pitch_dead_zone)) {
		point->y = gps.rest_y;
	} else {
		// use two different linear interpolation equations since the
		// resting coordinate is not always the center of the display
		if (axis_pitch > 0) {
			point->y = (((gps.max_y - gps.rest_y) * axis_pitch) >> 15) + gps.rest_y;
		} else {
			point->y = ((gps.rest_y * axis_pitch) >> 15) + gps.rest_y;
		}
		ret = GP_FLIGHT_UPDATE;
	}

	return ret;
}

/// \brief menu navigation support via conversion from stick coordinates to pointer coordinates
/// \param  stick input axis values
/// \param  point output simulated mouse pointer values
/// \return 0 if stick is in the dead zone or GP_NAV_UPDATE otherwise
uint16_t gamepad_axis_nav_conv(const vec2d_t *stick, pointer_sys_t *point)
{
	uint16_t ret = 0;
	int16_t axis_nav_ns = stick->x;
	int16_t axis_nav_ew = stick->y;

	if ((axis_nav_ns < gpc.axis_long_nav_dead_zone) && (axis_nav_ns > -gpc.axis_long_nav_dead_zone)) {
		// point->x remains unchanged
	} else {
		point->y += JOY_NAV_INC * (axis_nav_ns >> 13);
		ret = GP_NAV_UPDATE;
	}

	if ((axis_nav_ew < gpc.axis_trans_nav_dead_zone) && (axis_nav_ew > -gpc.axis_trans_nav_dead_zone)) {
		// point->y remains unchanged
	} else {
		point->x += JOY_NAV_INC * (axis_nav_ew >> 13);
		ret = GP_NAV_UPDATE;
	}

	return ret;
}

/// \brief menu navigation support via conversion from hat coordinates to pointer coordinates
/// \param  hat input value
/// \param  point output simulated mouse pointer values
/// \return 0 if stick is resting or GP_NAV_UPDATE otherwise
uint16_t gamepad_hat_nav_conv(const vec1d_t *hat, pointer_sys_t *point)
{
	return 0;
}

/// \brief longitudinal and transversal hat movement converted to hardcoded keyboard keypresses
/// \param  hat input value
void gamepad_hat_mov_conv(const vec1d_t *hat)
{
}

/// \brief longitudinal and transversal movement via conversion from stick coordinates to key presses
/// \param  stick input axis values
void gamepad_axis_mov_conv(vec2d_t *stick)
{
	uint16_t ret = 0;
	int16_t axis_long_inv = 1;
	int16_t axis_long = stick->x;
	int16_t axis_trans = stick->y;

	if (stick->x_conf & GAMEPAD_AXIS_INVERTED) {
		axis_long_inv = -1;
	}

	if ((axis_long < gpc.axis_long_dead_zone) && (axis_long > -gpc.axis_long_dead_zone)) {
		// player seems to always have some inertia, so the following wont't actually stop
		// longitudinal movement
		if (gps.mov_key_announced < GP_MAX_KEY_RELEASE_ANN) {
			setPress(false, GP_KEY_EMU_UP);
			setPress(false, GP_KEY_EMU_DOWN);
		}
	} else {
		if (axis_long * axis_long_inv > 0) {
			setPress(false, GP_KEY_EMU_DOWN);
			setPress(true, GP_KEY_EMU_UP);
		} else {
			setPress(false, GP_KEY_EMU_UP);
			setPress(true, GP_KEY_EMU_DOWN);
		}
		ret = GP_MOV_UPDATE;
	}

	if ((axis_trans < gpc.axis_trans_dead_zone) && (axis_trans > -gpc.axis_trans_dead_zone)) {
		if (gps.mov_key_announced < GP_MAX_KEY_RELEASE_ANN) {
			setPress(false, GP_KEY_EMU_RIGHT);
			setPress(false, GP_KEY_EMU_LEFT);
		}
	} else {
		if (axis_trans > 0) {
			setPress(false, GP_KEY_EMU_LEFT);
			setPress(true, GP_KEY_EMU_RIGHT);
		} else {
			setPress(false, GP_KEY_EMU_RIGHT);
			setPress(true, GP_KEY_EMU_LEFT);
		}
		ret = GP_MOV_UPDATE;
	}

	if (!ret) {
		if (gps.mov_key_announced < GP_MAX_KEY_RELEASE_ANN) {
			gps.mov_key_announced++;
		}
	} else {
		gps.mov_key_announced = 0;
	}
}

/// \brief button-like action via conversion from axis coordinates to a boolean (for xbox trigger buttons)
/// \param  input axis value
/// \return 0 is button is inside the dead zone, 1 otherwise
void gamepad_axis_bool_conv(const int16_t input, bool *ret)
{
	if (input > -32767 + gpc.trigger_dead_zone) {
		*ret = 1;
	} else {
		*ret = 0;
	}
}

/// \brief emulate a mouse based on data provided by a gamepad or joystick
/// \param gpe  gamepad_event_t event strucure populated thru SDL_PollEvent()
void gamepad_event_mgr(gamepad_event_t *gpe)
{
}

/// \brief once per frame read out all axes and hats, for perfect smoothness
/// \param gpe event data
void gamepad_poll_data(gamepad_event_t *gpe)
{
}

/// \brief reconfigure gamepad maximum coverage and operating mode based on recode scene
/// \param scene_id one of SCENE_PREAMBLE_MENU, SCENE_FLIGHT, SCENE_FLIGHT_MENU
void set_scene(const uint8_t scene_id)
{
	gps.scene_id = scene_id;
	switch (scene_id) {
		case SCENE_PREAMBLE_MENU:
			gps.max_x = 640;
			gps.max_y = 480;
			gps.nav_mode = 1;
			break;
		case SCENE_FLIGHT:
			gps.max_x = gameResWidth;
			gps.max_y = gameResHeight;
			gps.nav_mode = 0;
			break;
		case SCENE_FLIGHT_MENU:
		case SCENE_SPELL_MENU:
			gps.max_x = gameResWidth;
			gps.max_y = gameResHeight;
			gps.nav_mode = 1;
			break;
		default:
			gps.max_x = gameResWidth;
			gps.max_y = gameResHeight;
			break;
	}
	Logger->trace("set scene {}, nav_mode {}", scene_id, gps.nav_mode);
}

/// \brief set the x,y simulated mouse pointer coordinates of the joystick rest position
/// \param x coordinate
/// \param y coordinate
void joystick_set_env(const int32_t x, const int32_t y)
{
	Logger->trace("pointer rest at {},{} scene {}, window size {},{}", x, y, gps.scene_id, gps.max_x, gps.max_y);
	gps.rest_x = x;
	gps.rest_y = y;
	gps.x = x;
	gps.y = y;
}

/// \brief unfinished load effects to be sent to the haptic subsystem
/// \return EXIT_FAILURE on error, EXIT_SUCCESS otherwise
int8_t haptic_load_effects(void) {

	return EXIT_SUCCESS;
}

/// \brief send sine-based effect to the haptic subsystem
/// \param effect_id  identifier
void haptic_run_effect(const int effect_id) {

}

/// \brief send rumble effect to the haptic subsystem
/// \param strength defined in the [0-1.0] interval
/// \param length  effect duration in ms
void haptic_rumble_effect(const float strength, const uint32_t length) {
}

/// \brief send rumble effect to the trigger buttons
/// \param strength_l TL effect level
/// \param strength_r TR effect level
/// \param length effect duration in ms
void haptic_rumble_triggers_effect(const uint16_t strength_l, const uint16_t strength_r, const uint32_t length) {
	if ((!hs.enabled) || (!hs.rumble_trig)) {
		return;
	}
#ifdef _MSC_VER
	//SDL_JoystickRumbleTriggers(m_gameController, strength_l, strength_r, length);
#endif
}

