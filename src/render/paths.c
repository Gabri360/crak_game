#include "paths.h"
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

static int PathExists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0;
}


void GetResourcePath(const char* relativePath, char* outBuf, size_t outBufSize) {
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);


    if (len != -1) {
        exePath[len] = '\0';

        char exeDirBuf[1024];
        strncpy(exeDirBuf, exePath, sizeof(exeDirBuf) - 1);
        exeDirBuf[sizeof(exeDirBuf) - 1] = '\0';
        char* dir = dirname(exeDirBuf);

        char devPath[1024];
        snprintf(devPath, sizeof(devPath), "%s/%s", dir, relativePath);

        if (PathExists(devPath)) {
            snprintf(outBuf, outBufSize, "%s", devPath);
            return;
        }
    }
    snprintf(outBuf, outBufSize, "%s/%s", INSTALL_RESOURCES_DIR, relativePath);
}
