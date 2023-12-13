#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

char* ReadBinaryFile(const char* pFilename, int& size)
{
    FILE* f = nullptr;
    errno_t err = fopen_s(&f, pFilename, "rb");
    
    if (!f) {
        char buf[256] = { 0 };
        strerror_s(buf, sizeof(buf), err);
        exit(0);
    }

    struct stat stat_buf;
    int error = stat(pFilename, &stat_buf);
    
    if (error) {
        char buf[256] = { 0 };
        strerror_s(buf, sizeof(buf), err);
        return NULL;
    }

    size = stat_buf.st_size;

    char* p = (char*)malloc(size);

    size_t bytes_read = fread(p, 1, size, f);

    if (bytes_read != size) {
        char buf[256] = { 0 };
        strerror_s(buf, sizeof(buf), err);
        exit(0);
    }

    fclose(f);

    return p;
}

#endif