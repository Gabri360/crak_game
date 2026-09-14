#ifndef PATHS_H
#define PATHS_H

#include <stddef.h>


void GetResourcePath(const char* relativePath, char* outBuf, size_t outBufSize);


void GetUserDataPath(const char* filename, char* outBuf, size_t outBufSize);

#endif
