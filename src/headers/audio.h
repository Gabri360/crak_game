#ifndef AUDIO_H
#define AUDIO_H

void Audio_Init(void);
void Audio_PlayMusicLoop(const char* path, float volume);
void Audio_Shutdown(void);

#endif
