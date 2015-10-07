#ifndef __ESPRESSO_GUIDE_H_
#define __ESPRESSO_GUIDE_H_

typedef enum __screen_type
{
  /* 0: Welcome screen shows at the startup */
  SCREEN_WELCOME = 0,
  /* 1: Shows arrows and temp. T < Tmin */
  SCREEN_WARMUP,
  /* 2: Shows the espresso cup and temp. Tmin < T < Tsteam */
  SCREEN_ESPRESSO,
  /* 3: Shows the espresso symbol and temp. Tsteam < T < Tmax */
  SCREEN_STEAM,
  /* 4: Shows an error */
  SCREEN_ERROR,
  /* 5: Shows the big time counter and temp when the pump is on */
  SCREEN_PUMP,
  /* 6: Heating up for steam */
  SCREEN_HEATUP,
  /* 7: Temperature under warmup.min */
  SCREEN_TOOCOLD,
  /* 8: Temperature under warmup.min */
  SCREEN_TOOHOT,
} SCREEN_TYPE;

typedef enum __error_type
{
  ERROR_NONE = 0,
  ERROR_SENSOR
} ERROR_TYPE;

typedef struct __range
{
  double min;
  double max;
} RANGE;

typedef struct __temp_config_data
{
  RANGE warmup;
  RANGE espresso;
  RANGE steam;
  double max;
} TEMP_CONFIG;

typedef struct __current_state
{
  /* unfiltered temperature value */
  double temperature_fast;
  /* filtered, slow temperature value */
  double temperature;
  /* pump state (1 = on, 0 = off) */
  unsigned char pump;
  /* temperature range for display */
  RANGE range;
  /* current error */
  ERROR_TYPE error;
  /* current screen */
  SCREEN_TYPE screen;
} CURRENT_STATE;

typedef struct __action_counter
{
  unsigned long count;
  unsigned int interval;
} ACTION_COUNTER;

#endif
