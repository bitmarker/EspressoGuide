#ifndef __ESPRESSO_GUIDE_H_
#define __ESPRESSO_GUIDE_H_

#include <stdint.h>

#define MEAS_DATA_MAXLEN 8

typedef enum __screen_type
{
  SCREEN_WELCOME = 0,
  SCREEN_BREW,
  SCREEN_IDLE,
  SCREEN_ERROR,
  SCREEN_TEMPERATURE
} SCREEN_TYPE;

typedef enum __shot_state
{
  SHOT_IDLE = 0,
  SHOT_BREWING
} SHOT_STATE;

typedef enum __error_type
{
  ERROR_NONE = 0,
  ERROR_SENSOR
} ERROR_TYPE;

typedef struct __time
{
  unsigned int hours;
  unsigned int minutes;
  unsigned int seconds;
} TIME;

typedef struct __point
{
  uint16_t x;
  uint16_t y;  
} POINT;

typedef struct __meas_data
{
    double values[MEAS_DATA_MAXLEN];
    unsigned int length;
    double average;
    unsigned char full;
    double last_sum;
    double slope;
} MEAS_DATA;

typedef struct __current_state
{
  /* Unfiltered temperature value */
  double temperature_fast;
  
  /* Filtered, slow temperature value */
  double temperature;
  
  /* last temperature value */
  double last_temperature;
  
  /* Pump state (1 = on, 0 = off) */
  unsigned char pump;
  
  /* Current error */
  ERROR_TYPE error;
  
  /* Current screen */
  SCREEN_TYPE screen;
  
  /* Time since power on */
  TIME run_time;
  
  /* Time since the pump was turned on */
  TIME brew_time;
  
  /* Shot was done */
  SHOT_STATE shot_state;
  
  /* Delay before the brew counter starts */
  unsigned char brew_counter_delay;
  
  /* Temperature trend (-1, 0, 1) */
  int temp_trend;
  
  /* Counter for blinking animation */
  unsigned char blink_counter;
  
  /* Counter for the welcome screen */
  unsigned char welcome_counter;
  
  /* Temperature data */
  MEAS_DATA temp_data;
} CURRENT_STATE;

typedef void (*ACTION_COUNTER_CALLBACK)(CURRENT_STATE*);

typedef struct __action_counter
{
  unsigned long count;
  unsigned int interval;
  unsigned char elapsed;
  ACTION_COUNTER_CALLBACK callback;
} ACTION_COUNTER;



#endif

