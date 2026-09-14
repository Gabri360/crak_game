#include "paths.h"
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

static int PathExists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static void MkdirRecursive(const char* path) {
    char tmp[768];
    snprintf(tmp, sizeof(tmp), "%s", path);
    size_t len = strlen(tmp);
    if (len > 0 && tmp[len - 1] == '/') tmp[len - 1] = '\0';

    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
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

void GetUserDataPath(const char* filename, char* outBuf, size_t outBufSize) {
    char baseDir[768];
    const char* xdgDataHome = getenv("XDG_DATA_HOME");

    if (xdgDataHome && xdgDataHome[0] != '\0') {
        snprintf(baseDir, sizeof(baseDir), "%s/crak", xdgDataHome);
    } else {
        const char* home = getenv("HOME");
        if (!home) home = ".";
        snprintf(baseDir, sizeof(baseDir), "%s/.local/share/crak", home);
    }

    MkdirRecursive(baseDir);
    snprintf(outBuf, outBufSize, "%s/%s", baseDir, filename);
}
