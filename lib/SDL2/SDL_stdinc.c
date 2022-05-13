#include "SDL_stdinc.h"

size_t SDL_strlcpy(char *dst, const char *src, size_t maxlen) {
	strncpy(dst, src, maxlen);
	return strlen(src);
}

int SDL_strcasecmp(const char *str1, const char *str2) {
	return strcmp(str1, str2);
}