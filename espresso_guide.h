#ifndef __ESPRESSO_GUIDE_H_
#define __ESPRESSO_GUIDE_H_

typedef enum __screen_type
{
  /* Welcome screen shows at the startup */
  SCREEN_WELCOME = 0,
  /* Shows arrows and temp. T < Tmin */
  SCREEN_WARMUP,
  /* Shows the espresso cup and temp. Tmin < T < Tsteam */
  SCREEN_ESPRESSO,
  /* Shows the espresso symbol and temp. Tsteam < T < Tmax */
  SCREEN_STEAM,
  /* Shows an error */
  SCREEN_ERROR,
  /* Shows the big time counter and temp when the pump is on */
  SCREEN_PUMP,
  /* Heating up for steam */
  SCREEN_HEATUP
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

#endif
