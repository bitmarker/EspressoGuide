#ifndef _UTILS_H_
#define _UTILS_H_

#include "espresso_guide.h"

#ifdef __cplusplus
extern "C" {
#endif

void initTime(TIME *t);
void formatTime(TIME *t, char *buffer);
void incrementSeconds(TIME *t);
unsigned long timeInSeconds(TIME *t);
void initCounter(ACTION_COUNTER *counter, unsigned int interval, ACTION_COUNTER_CALLBACK callback);
void copy_range(RANGE *from, RANGE *to);

#ifdef __cplusplus
}
#endif

#endif
