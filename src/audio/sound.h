#pragma once
#include <SDL3/SDL.h>

namespace Audio {

void init();
void close();

struct Sound {
  SDL_AudioSpec spec;
  Uint8 *start;
  Uint32 length;
};
Sound loadSound(const char *file);
void unloadSound(Sound &sound);

void playSound(const Sound &sound);

} // namespace Audio
