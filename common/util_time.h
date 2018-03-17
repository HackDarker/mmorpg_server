#ifndef TIME_COMMON_H
#define TIME_COMMON_H

#include <cstddef>
#include <sys/time.h>
#include <stdint.h>
#include <time.h>

uint32_t getMSTime();
void updateFrameTime();
uint32_t getFrameTime();
void Sleep(uint32_t ms);

#endif