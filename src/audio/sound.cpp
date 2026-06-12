#include "sound.h"

#include <SDL3/SDL_audio.h>

#include <iostream>

namespace Audio {

namespace {

SDL_AudioDeviceID device;
SDL_AudioSpec spec;
SDL_AudioStream *stream = nullptr;
bool audioEnabled;

} // namespace

void init() {
  int count;
  auto devices = SDL_GetAudioPlaybackDevices(&count);
  if (count == 0) {
    audioEnabled = false;
    std::cout << "Could not find an audio device\n";
    return;
  }
  audioEnabled = true;

  device = SDL_OpenAudioDevice(devices[0], NULL);
  if (not device) {
    audioEnabled = false;
    std::cout << "Could not open audio device\n";
    return;
  }

  spec.freq = 24000;
  spec.channels = 2;
  spec.format = SDL_AUDIO_S16LE;

  stream = SDL_CreateAudioStream(&spec, NULL);
  if (not stream) {
    audioEnabled = false;
    std::cout << "Could not create audio stream\n";
    return;
  }

  if (not SDL_BindAudioStream(device, stream)) {
    std::cout << "Failed to bind audio stream\n";
    std::cout << SDL_GetError() << '\n';
    audioEnabled = false;
    return;
  }

  if (not SDL_SetAudioStreamGain(stream, 0.3f)) {
    std::cout << "Could not set audio stream volume\n";
  }
}

void close() {
  if (not audioEnabled)
    return;

  if (stream) {
    SDL_DestroyAudioStream(stream);
    stream = nullptr;
  }
  SDL_CloseAudioDevice(device);
}

Sound loadSound(const char *file) {
  if (not audioEnabled)
    return {};

  Sound result;
  if (not SDL_LoadWAV(file, &result.spec, &result.start, &result.length)) {
    std::cout << "Failed to load WAV file\n";
  }
  return result;
}

void unloadSound(Sound &sound) {
  if (not audioEnabled)
    return;

  SDL_free(sound.start);
}

void playSound(const Sound &sound) {
  if (not audioEnabled)
    return;

  SDL_ClearAudioStream(stream);
  if (not SDL_PutAudioStreamData(stream, sound.start, sound.length)) {
    std::cout << "Failed to enqueue audio data\n";
  }
}

} // namespace Audio
