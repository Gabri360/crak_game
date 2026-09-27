#include "paths.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <unistd.h>
    #define MKDIR(path) mkdir(path, 0755)
#endif

static int PathExists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static void MkdirRecursive(const char* path) {
    char tmp[768];
    snprintf(tmp, sizeof(tmp), "%s", path);
    size_t len = strlen(tmp);
    if (len > 0 && (tmp[len - 1] == '/' || tmp[len - 1] == '\\')) tmp[len - 1] = '\0';

    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char sep = *p;
            *p = '\0';
            MKDIR(tmp);
            *p = sep;
        }
    }
    MKDIR(tmp);
}

static void GetDirectoryPart(const char* path, char* outDir, size_t outSize) {
    size_t len = strlen(path);
    long lastSep = -1;

    for (long i = (long)len - 1; i >= 0; i--) {
        if (path[i] == '/' || path[i] == '\\') {
            lastSep = i;
            break;
        }
    }

    if (lastSep < 0) {
        snprintf(outDir, outSize, ".");
        return;
    }

    size_t copyLen = (size_t)lastSep;
    if (copyLen >= outSize) copyLen = outSize - 1;
    memcpy(outDir, path, copyLen);
    outDir[copyLen] = '\0';
}

static int GetExecutablePath(char* outBuf, size_t outBufSize) {
#ifdef _WIN32
    DWORD len = GetModuleFileNameA(NULL, outBuf, (DWORD)outBufSize);
    if (len == 0 || len == outBufSize) return 0;
    return 1;
#else
    ssize_t len = readlink("/proc/self/exe", outBuf, outBufSize - 1);
    if (len == -1) return 0;
    outBuf[len] = '\0';
    return 1;
#endif
}

void GetResourcePath(const char* relativePath, char* outBuf, size_t outBufSize) {
    char exePath[1024];

    if (GetExecutablePath(exePath, sizeof(exePath))) {
        char exeDir[1024];
        GetDirectoryPart(exePath, exeDir, sizeof(exeDir));

        char devPath[1280];

        #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat-truncation"
        #endif
        snprintf(devPath, sizeof(devPath), "%s/%s", exeDir, relativePath)
			;
        #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic pop
        #endif

        if (PathExists(devPath)) {
            snprintf(outBuf, outBufSize, "%s", devPath);
            return;
        }
    }

    snprintf(outBuf, outBufSize, "%s/%s", INSTALL_RESOURCES_DIR, relativePath);
}

void GetUserDataPath(const char* filename, char* outBuf, size_t outBufSize) {
    char baseDir[768];

#ifdef _WIN32
    const char* appData = getenv("APPDATA");
    if (appData && appData[0] != '\0') {
        snprintf(baseDir, sizeof(baseDir), "%s/crak", appData);
    } else {
        snprintf(baseDir, sizeof(baseDir), "./crak_data");
    }
#else
    const char* xdgDataHome = getenv("XDG_DATA_HOME");
    if (xdgDataHome && xdgDataHome[0] != '\0') {
        snprintf(baseDir, sizeof(baseDir), "%s/crak", xdgDataHome);
    } else {
        const char* home = getenv("HOME");
        if (!home) home = ".";
        snprintf(baseDir, sizeof(baseDir), "%s/.local/share/crak", home);
    }
#endif

    MkdirRecursive(baseDir);
    snprintf(outBuf, outBufSize, "%s/%s", baseDir, filename);
}
