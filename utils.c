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


#include "utils.h"
#include <stdio.h>

/**
 * Resets the time structure
 */
void initTime(TIME *t)
{
  t->seconds = 0;
  t->hours = 0;
  t->minutes = 0;
}

/**
 * Formats the time as a string for the top bar
 */
void formatTime(TIME *t, char *buffer)
{
  /* Show the hours only when greater than 0 */
  if (t->hours > 0)
  {
    sprintf(buffer, "%d:%02d:%02d", t->hours, t->minutes, t->seconds);
  }
  else
  {
    sprintf(buffer, "%02d:%02d", t->minutes, t->seconds);
  }
}


/**
 * Increments the seconds of a time structure. If the seconds count is greater than
 * 59, minutes will be counted (and also hours)
 */
void incrementSeconds(TIME *t)
{
  t->seconds++;

  if (t->seconds >= 60)
  {
    t->minutes++;
    t->seconds = 0;
  }

  if (t->minutes >= 60)
  {
    t->hours++;
    t->minutes = 0;
  }

  if (t->hours >= 10)
  {
    t->hours = 0;
    t->minutes = 0;
    t->seconds = 0;
  }
}


/**
 * Converts a time structure to seconds
 */
unsigned long timeInSeconds(TIME *t)
{
  return t->seconds + t->minutes * 60 + t->hours * 3600;
}


/*
 * Resets a time counter
 */
void initCounter(ACTION_COUNTER *counter, unsigned int interval, ACTION_COUNTER_CALLBACK callback)
{
  counter->elapsed = 0;
  counter->interval = interval;
  counter->count = 0;
  counter->callback = callback;
}

/***
 * Adding the next measured value and calculatong the linear regression
 * for determining the trend.
 */
void addNextValue(MEAS_DATA *data, double value)
{
  unsigned int i = 0;
  double x_average = 0;
  double sum1 = 0;
  double sum2 = 0;
  double x_temp;

  if (data->length < MEAS_DATA_MAXLEN)
  {
    data->length++;
  }

  /* Clculating the average value */
  data->average = (data->last_sum + value) / (data->length);

  x_average = (data->length - 1) / 2.0;

  data->last_sum = 0;

  for (i = 0; i < data->length; i++)
  {
    /* Shift the values to the left */
    if (i < (data->length - 1) && data->full)
    {
      data->values[i] = data->values[i + 1];
    }
    else
    {
      data->values[data->length - 1] = value;
    }

    /* Store the sum without the first element */
    if (i > 0 || data->length < MEAS_DATA_MAXLEN)
    {
      data->last_sum += data->values[i];
    }

    /* Calculating the least squares */
    x_temp = (i - x_average);
    sum1 += x_temp * (data->values[i] - data->average);
    sum2 += x_temp * x_temp;
  }

  /* Calculate the slope */
  if (sum2 > 0)
  {
    data->slope = sum1 / sum2;
  }
  else
  {
    data->slope = 0;
  }

  if (!data->full)
  {
    data->full = data->length == MEAS_DATA_MAXLEN;
  }
}

void initMeasData(MEAS_DATA *data)
{
  data->length = 0;
  data->full = 0;
  data->average = 0;
  data->last_sum  = 0;
  data->slope = 0;
}

uint8_t getNumberDigits(uint16_t number, uint8_t *digits)
{
  uint16_t delimiter[] = {10000, 1000, 100, 10, 1};
  uint8_t index;
  uint8_t count = 0;
  uint16_t temp = number;

  for (index = 0; index < sizeof(delimiter) / sizeof(uint16_t); index++)
  {
    digits[count] = (uint8_t)(temp / delimiter[index]);

    temp -= digits[count] * delimiter[index];

    if (digits[count] != 0 || count != 0)
    {
      count++;
    }
  }

  if (number < 10)
  {
    count = 2;
    digits[1] = digits[0];
    digits[0] = 0;
  }

  return count;
}
