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
  if(t->hours > 0)
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
  
  if(t->seconds >= 60)
  {
    t->minutes++;
    t->seconds = 0;
  }
  
  if(t->minutes >= 60)
  {
    t->hours++;
    t->minutes = 0;
  }

  if(t->hours >= 10)
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


void copy_range(RANGE *from, RANGE *to)
{
  to->min = from->min;
  to->max = from->max; 
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
        
    if(data->length < MEAS_DATA_MAXLEN)
    {
        data->length++;
    }

    /* Clculating the average value */
    data->average = (data->last_sum + value) / (data->length);
    
    x_average = (data->length - 1) / 2.0;
    
    data->last_sum = 0;
    
    for(i = 0; i < data->length; i++)
    {        
        /* Shift the values to the left */
        if(i < (data->length - 1) && data->full)
        {
            data->values[i] = data->values[i + 1];
        }
        else
        {
            data->values[data->length - 1] = value;
        }
        
        /* Store the sum without the first element */
        if(i > 0 || data->length < MEAS_DATA_MAXLEN)
        {
            data->last_sum += data->values[i];
        }

        /* Calculating the least squares */
        x_temp = (i - x_average);
        sum1 += x_temp * (data->values[i] - data->average);
        sum2 += x_temp * x_temp;
    }

    /* Calculate the slope */
    if(sum2 > 0)
    {
        data->slope = sum1/sum2;
    }
    else
    {
        data->slope = 0;
    }

    if(!data->full)
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
