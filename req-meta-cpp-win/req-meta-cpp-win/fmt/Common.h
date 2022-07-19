#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <functional>

#define DEC_BIN_SEARCH(NAME, TYPE, COMP)                       \
inline int NAME(TYPE *arr, int len, TYPE *search, int *out_idx) {    \
   int c, first = 0, last = len - 1, middle;                   \
   if (len == 0) { *out_idx = 0; return 0; }                   \
   while (first <= last) {                                     \
       middle = (first + last)/2;                              \
       c = COMP(&arr[middle], search);                         \
       if (c < 0) first = middle + 1;                          \
       else if (c == 0) break;                                 \
       else last = middle - 1;                                 \
   }                                                           \
   if (first > last) {                                         \
       if (c > 0) *out_idx = middle;                           \
       else *out_idx = middle + 1;                             \
       return 0;                                               \
   } else {                                                    \
       *out_idx = middle; return 1;                            \
   }                                                           \
}

#define DEBUG

class Logger {
public:
    void error(FILE* fp, const char* fmt, ...)
    {
        if (fp == NULL) return;

        fprintf(fp, "[E] ");

        va_list args;
        va_start(args, fmt);
        vfprintf(fp, fmt, args);
        va_end(args);

        fprintf(fp, "\n");
        fflush(fp);
    }
    void debug(FILE *fp, const char* fmt, ...)
    {
        if (fp == NULL) return;
#ifdef DEBUG
        fprintf(fp, "[D] ");

        va_list args;
        va_start(args, fmt);
        vfprintf(fp, fmt, args);
        va_end(args);

        fprintf(fp, "\n");
        fflush(fp);
#endif
    }

private:
    FILE* _fp = NULL;
};

Logger* getLogger();

#define LOGD(FMT, ...) \
    getLogger()->debug(stdout, FMT, __VA_ARGS__)

#define LOGE(FMT, ...) \
    getLogger()->error(stdout, FMT, __VA_ARGS__)
