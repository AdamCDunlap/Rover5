#include "Utility.h"

uint32_t micros() {
    struct timeval time;
    gettimeofday(&time, nullptr);
    return time.tv_sec + (time.tv_usec * 1000000);
}
