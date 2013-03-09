#include "Utility.h"

static uint32_t curtime = 0;

uint32_t micros() {
    struct timeval time;
    gettimeofday(&time, nullptr);
    return (uint32_t)time.tv_usec + (uint32_t)((uint32_t)time.tv_sec * (uint32_t)1000000);
}

void setTime(uint32_t tm) {
    curtime = tm;
}

