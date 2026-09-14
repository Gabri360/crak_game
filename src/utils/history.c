#include "history.h"
#include "paths.h"
#include <stdio.h>

#define HISTORY_FILENAME "scores.csv"

void History_AddScore(int score) {
    char path[512];
    GetUserDataPath(HISTORY_FILENAME, path, sizeof(path));

    FILE* f = fopen(path, "a");
    if (!f) {
        fprintf(stderr, "[Error] unable write history: %s\n", path);
        return;
    }

    time_t now = time(NULL);
    fprintf(f, "%d,%ld\n", score, (long)now);
    fclose(f);
}

size_t History_LoadAll(ScoreEntry* outEntries, size_t maxEntries) {
    char path[512];
    GetUserDataPath(HISTORY_FILENAME, path, sizeof(path));

    FILE* f = fopen(path, "r");
    if (!f) {
        return 0;
    }

    size_t count = 0;
    char line[256];
    while (count < maxEntries && fgets(line, sizeof(line), f)) {
        int score;
        long ts;
        if (sscanf(line, "%d,%ld", &score, &ts) == 2) {
            outEntries[count].score = score;
            outEntries[count].timestamp = (time_t)ts;
            count++;
        }
    }

    fclose(f);
    return count;
}
