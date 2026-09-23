#include "audio.h"
#include "miniaudio.h"
#include <stdio.h>

static ma_engine engine;
static ma_sound musicSound;
static int engineInitialized = 0;
static int musicLoaded = 0;

void Audio_Init(void) {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "[Error] unable to load audio motor\n");
        return;
    }
    engineInitialized = 1;
}

void Audio_PlayMusicLoop(const char* path, float volume) {
    if (!engineInitialized) return;

    ma_result result = ma_sound_init_from_file(&engine, path, MA_SOUND_FLAG_STREAM, NULL, NULL, &musicSound);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "[Error] unable to load audio: %s\n", path);
        return;
    }

    ma_sound_set_looping(&musicSound, MA_TRUE);
    ma_sound_set_volume(&musicSound, volume);
    ma_sound_start(&musicSound);
    musicLoaded = 1;
}

void Audio_Shutdown(void) {
    if (musicLoaded) {
        ma_sound_uninit(&musicSound);
    }
    if (engineInitialized) {
        ma_engine_uninit(&engine);
    }
}
