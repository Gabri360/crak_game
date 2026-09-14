#ifndef HISTORY_H
#define HISTORY_H

#include <stddef.h>
#include <time.h>

typedef struct {
    int score;
    time_t timestamp;
} ScoreEntry;

void History_AddScore(int score);
size_t History_LoadAll(ScoreEntry* outEntries, size_t maxEntries);

#endif
