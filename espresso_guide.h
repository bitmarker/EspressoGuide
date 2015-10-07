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
  SCREEN_PUMP
} SCREEN_TYPE;


#endif
