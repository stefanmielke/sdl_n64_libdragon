#include "SDL_joystick.h"

#include <libdragon.h>

struct _SDL_Joystick {};

struct controller_data keys_pressed;
struct controller_data keys_down;

int SDL_NumJoysticks(void) {
	int controllers = get_controllers_present();

	int count = 0;
	if (controllers & CONTROLLER_1_INSERTED)
		++count;
	if (controllers & CONTROLLER_2_INSERTED)
		++count;
	if (controllers & CONTROLLER_3_INSERTED)
		++count;
	if (controllers & CONTROLLER_4_INSERTED)
		++count;

	return count;
}

SDL_Joystick *SDL_JoystickOpen(int device_index) {
	return (SDL_Joystick *)device_index;
}

void SDL_JoystickClose(SDL_Joystick *joystick) {
}

const char *SDL_JoystickName(SDL_Joystick *joystick) {
	return "N64 Pad";
}

int SDL_JoystickNumAxes(SDL_Joystick *joystick) {
	// one for X, one for Y
	return 2;
}

int SDL_JoystickNumHats(SDL_Joystick *joystick) {
	return 1;
}

int SDL_JoystickNumButtons(SDL_Joystick *joystick) {
	return 10;
}

void SDL_JoystickUpdate(void) {
	controller_scan();
	keys_pressed = get_keys_pressed();
	keys_down = get_keys_down();
}

#define BT_PRESSED(button) keys_pressed.c[pad_id].button

Uint8 SDL_JoystickGetButton(SDL_Joystick *joystick, int button) {
	int pad_id = (int)joystick;

	switch (button) {
		case 0:
			return BT_PRESSED(A);
		case 1:
			return BT_PRESSED(B);
		case 2:
			return BT_PRESSED(Z);
		case 3:
			return BT_PRESSED(start);
		case 4:
			return BT_PRESSED(L);
		case 5:
			return BT_PRESSED(R);
		case 6:
			return BT_PRESSED(C_up);
		case 7:
			return BT_PRESSED(C_down);
		case 8:
			return BT_PRESSED(C_left);
		case 9:
			return BT_PRESSED(C_right);
		default:
			return 0;
	}
}

Uint8 SDL_JoystickGetHat(SDL_Joystick *joystick, int hat) {
	int pad_id = (int)joystick;

	Uint8 value = SDL_HAT_CENTERED;
	if (BT_PRESSED(left))
		value |= SDL_HAT_LEFT;
	if (BT_PRESSED(right))
		value |= SDL_HAT_RIGHT;
	if (BT_PRESSED(down))
		value |= SDL_HAT_DOWN;
	if (BT_PRESSED(up))
		value |= SDL_HAT_UP;

	return value;
}

Sint16 SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis) {
	int pad_id = (int)joystick;

	const int max_axis_value = 70;
	const float multiplier = (float)SDL_JOYSTICK_AXIS_MAX / max_axis_value;

	int value = 0;
	if (axis == 0) {
		value = fmin((int)keys_pressed.c[pad_id].x, max_axis_value);
	} else {
		value = fmin((int)keys_pressed.c[pad_id].y, max_axis_value);
	}

	return (Sint16)(multiplier * value);
}

int SDL_JoystickEventState(int state) {
	// TODO: auto polling should be called somewhere if enabled (enabled is default)
	return 0;
}
