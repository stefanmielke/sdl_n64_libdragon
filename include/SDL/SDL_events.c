#include "SDL_events.h"

#include <SDL/SDL.h>
#include <libdragon.h>

#ifndef EVENT_BUFFER_SIZE
#define EVENT_BUFFER_SIZE 10  // size of the event buffer
#endif
#ifndef EVENT_GEN_KEYBOARD_EVENTS
#define EVENT_GEN_KEYBOARD_EVENTS 1	 // if you want to generate keyboard events for the gamepad
#endif

extern bool gen_event_flag;

// works as a ring buffer
static SDL_Event event_buffer[EVENT_BUFFER_SIZE];
static int current_event_pointer = 0;
static int next_event_pointer = 0;

#define KEY_EVENT(SCANCODE)                                                                        \
	{                                                                                              \
		SDL_Event key_event;                                                                       \
		key_event.type = event_type;                                                               \
		key_event.key.keysym.sym = (SCANCODE);                                                     \
		key_event.key.state = (event_type == SDL_KEYUP) ? SDL_RELEASED : SDL_PRESSED;              \
		SDL_PushEvent(&key_event);                                                                 \
	}

#define GP_EVENT(BUTTON)                                                                           \
	{                                                                                              \
		SDL_Event event;                                                                           \
		event.type = SDL_KEYUP ? SDL_JOYBUTTONUP : SDL_JOYBUTTONDOWN;                              \
		event.jbutton.which = c_num;                                                               \
		event.jbutton.state = SDL_KEYUP ? SDL_RELEASED : SDL_PRESSED;                              \
		event.jbutton.button = BUTTON;                                                             \
		SDL_PushEvent(&event);                                                                     \
	}

static void gen_input_events_for_contr(int c_num, int event_type, struct controller_data *data) {
	if (data->c[c_num].A) {
#if EVENT_GEN_KEYBOARD_EVENTS == 1
		KEY_EVENT(SDLK_SPACE);
#endif
		GP_EVENT(SDL_CONTROLLER_BUTTON_A);
	}
	if (data->c[c_num].left) {
#if EVENT_GEN_KEYBOARD_EVENTS == 1
		KEY_EVENT(SDLK_LEFT);
#endif
		GP_EVENT(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	}
	if (data->c[c_num].right) {
#if EVENT_GEN_KEYBOARD_EVENTS == 1
		KEY_EVENT(SDLK_RIGHT);
#endif
		GP_EVENT(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	}
	if (data->c[c_num].up) {
#if EVENT_GEN_KEYBOARD_EVENTS == 1
		KEY_EVENT(SDLK_UP);
#endif
		GP_EVENT(SDL_CONTROLLER_BUTTON_DPAD_UP);
	}
	if (data->c[c_num].down) {
#if EVENT_GEN_KEYBOARD_EVENTS == 1
		KEY_EVENT(SDLK_DOWN);
#endif
		GP_EVENT(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	}
}

static void gen_input_events() {
	controller_scan();
	struct controller_data keys_up = get_keys_up();
	struct controller_data keys_down = get_keys_down();

	int connected_controllers = get_controllers_present();
	if (connected_controllers & CONTROLLER_1_INSERTED) {
		gen_input_events_for_contr(0, SDL_KEYUP, &keys_up);
		gen_input_events_for_contr(0, SDL_KEYDOWN, &keys_down);
	}
	if (connected_controllers & CONTROLLER_2_INSERTED) {
		gen_input_events_for_contr(1, SDL_KEYUP, &keys_up);
		gen_input_events_for_contr(1, SDL_KEYDOWN, &keys_down);
	}
	if (connected_controllers & CONTROLLER_3_INSERTED) {
		gen_input_events_for_contr(2, SDL_KEYUP, &keys_up);
		gen_input_events_for_contr(2, SDL_KEYDOWN, &keys_down);
	}
	if (connected_controllers & CONTROLLER_4_INSERTED) {
		gen_input_events_for_contr(3, SDL_KEYUP, &keys_up);
		gen_input_events_for_contr(3, SDL_KEYDOWN, &keys_down);
	}
}

// We ignore "RegisterEvents" here and return everything
int SDL_PollEvent(SDL_Event *event) {
	// will generate necessary events (eg.: input) once per frame
	if (gen_event_flag) {
		gen_event_flag = !gen_event_flag;

		gen_input_events();
	}

	if (current_event_pointer == next_event_pointer) {
		return 0;
	}

	// always grab oldest event
	*event = event_buffer[current_event_pointer];

	// moving event pointer to the next
	++current_event_pointer;
	if (current_event_pointer >= EVENT_BUFFER_SIZE) {
		current_event_pointer = 0;
	}

	return 1;
}

// do not wait (we don't do async)
int SDL_WaitEvent(SDL_Event *event) {
	return 0;
}

// We ignore "RegisterEvents" here and accept anything
int SDL_PushEvent(SDL_Event *event) {
	event_buffer[next_event_pointer] = *event;

	++next_event_pointer;
	if (next_event_pointer >= EVENT_BUFFER_SIZE) {
		next_event_pointer = 0;
	}

	return 1;
}
