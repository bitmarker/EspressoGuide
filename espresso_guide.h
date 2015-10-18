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
  SCREEN_BREW,
  /* 6: Heating up for steam */
  SCREEN_HEATUP,
  /* 7: Temperature under warmup.min */
  SCREEN_TOOCOLD,
  /* 8: Temperature under warmup.min */
  SCREEN_TOOHOT,
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
  unsigned int shot_min_time;
  unsigned char brew_counter_delay;
  double trend_delta;
} CONFIG;

typedef struct __time
{
  unsigned int hours;
  unsigned int minutes;
  unsigned int seconds;
} TIME;

typedef struct __point
{
  unsigned int x;
  unsigned int y;  
} POINT;

typedef struct __icon
{
  unsigned int *pixels;
  unsigned int count;
} ICON;

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
  /* Temperature range for display */
  RANGE range;
  /* Current error */
  ERROR_TYPE error;
  /* Current screen */
  SCREEN_TYPE screen;
  /* Number of shots */
  unsigned int shot_count;
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
} CURRENT_STATE;

typedef void (*ACTION_COUNTER_CALLBACK)(CURRENT_STATE*);

typedef struct __action_counter
{
  unsigned long count;
  unsigned int interval;
  unsigned char elapsed;
  ACTION_COUNTER_CALLBACK callback;
} ACTION_COUNTER;

#define SET_ICON(screen, icon)   \
  icons[screen].pixels = icon; \
  icons[screen].count = sizeof(icon) / sizeof(unsigned int);

#endif
