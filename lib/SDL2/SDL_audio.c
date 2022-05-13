#include "SDL_audio.h"

/*
	For now we do not support audio.
*/

SDL_AudioDeviceID SDL_OpenAudioDevice(const char *device, int iscapture,
									  const SDL_AudioSpec *desired, SDL_AudioSpec *obtained,
									  int allowed_changes) {
	return 1;
}

void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on) {
}

void SDL_CloseAudioDevice(SDL_AudioDeviceID dev) {
}

void SDL_LockAudioDevice(SDL_AudioDeviceID dev) {
}

void SDL_UnlockAudioDevice(SDL_AudioDeviceID dev) {
}