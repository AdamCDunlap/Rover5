#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <sys/time.h>

#define TWO_PI 6.283185307179586476925286766559

uint32_t micros();

void setTime(uint32_t tm);

#endif//UTILITY_H
