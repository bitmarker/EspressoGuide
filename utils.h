/*
  EspressoGuide
  Copyright (C) 2015 Leonid Lezner

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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
void addNextValue(MEAS_DATA *data, double value);
void initMeasData(MEAS_DATA *data);
uint8_t getNumberDigits(uint16_t number, uint8_t *digits);

#ifdef __cplusplus
}
#endif

#endif

