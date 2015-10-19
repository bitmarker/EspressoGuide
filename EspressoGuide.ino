#include "espresso_guide.h"
#include <MicroView.h>
#include <SPI.h>
#include <Adafruit_MAX31855.h>

CONFIG config;
CURRENT_STATE current_state;

#define MAX_ICONS             10
#define MAX_COUNTERS          6
#define PIN_PUMP_MEAS         A0
#define PIN_DO                3
#define PIN_CS                6
#define PIN_CLK               5
#define WELCOME_SCREEN_DELAY  3

ACTION_COUNTER counters[MAX_COUNTERS];

ICON icons[MAX_ICONS];

int SCREEN_WIDTH = uView.getLCDWidth();
int SCREEN_HEIGHT = uView.getLCDHeight();

Adafruit_MAX31855 thermocouple(PIN_CLK, PIN_CS, PIN_DO);

const unsigned int icon_data_espresso_mug[] PROGMEM = {0x0611, 0x0708, 0x0709, 0x070a, 0x070b, 0x0711, 0x0801, 0x0805, 0x0808, 0x080a, 0x080b, 0x080c, 0x080d, 0x0811, 0x0902, 0x0904, 0x0906, 0x0908, 0x0909, 0x090a, 0x090c, 0x090d, 0x090e, 0x0911, 0x0a03, 0x0a08, 0x0a0a, 0x0a0b, 0x0a0c, 0x0a0e, 0x0a0f, 0x0a11, 0x0b08, 0x0b09, 0x0b0a, 0x0b0c, 0x0b0d, 0x0b0e, 0x0b0f, 0x0b11, 0x0c02, 0x0c05, 0x0c08, 0x0c0a, 0x0c0b, 0x0c0c, 0x0c0e, 0x0c0f, 0x0c11, 0x0d03, 0x0d04, 0x0d06, 0x0d08, 0x0d09, 0x0d0a, 0x0d0c, 0x0d0d, 0x0d0e, 0x0d0f, 0x0d11, 0x0e08, 0x0e0a, 0x0e0b, 0x0e0c, 0x0e0e, 0x0e0f, 0x0e11, 0x0f01, 0x0f05, 0x0f08, 0x0f09, 0x0f0a, 0x0f0b, 0x0f0c, 0x0f0d, 0x0f0e, 0x0f0f, 0x0f11, 0x1002, 0x1004, 0x1006, 0x1008, 0x1009, 0x100a, 0x100b, 0x100c, 0x100d, 0x100e, 0x1011, 0x1103, 0x1108, 0x1109, 0x110a, 0x110b, 0x110c, 0x110d, 0x1111, 0x1208, 0x1209, 0x120a, 0x120b, 0x120c, 0x1211, 0x1308, 0x130c, 0x1311, 0x1408, 0x140c, 0x1509, 0x150a, 0x150b};
const unsigned int icon_data_warmup[] PROGMEM = {0x0309, 0x030f, 0x0408, 0x0409, 0x040e, 0x040f, 0x0507, 0x0508, 0x0509, 0x050d, 0x050e, 0x050f, 0x0606, 0x0607, 0x0608, 0x0609, 0x060c, 0x060d, 0x060e, 0x060f, 0x0705, 0x0706, 0x0707, 0x0708, 0x0709, 0x070b, 0x070c, 0x070d, 0x070e, 0x070f, 0x0806, 0x0807, 0x0808, 0x0809, 0x080c, 0x080d, 0x080e, 0x080f, 0x0907, 0x0908, 0x0909, 0x090d, 0x090e, 0x090f, 0x0a08, 0x0a09, 0x0a0e, 0x0a0f, 0x0b09, 0x0b0f, 0x0e09, 0x0e0a, 0x0e0f, 0x0f06, 0x0f09, 0x0f0a, 0x0f0b, 0x0f0c, 0x0f0f, 0x1005, 0x1007, 0x1009, 0x100a, 0x100b, 0x100c, 0x100d, 0x100f, 0x1109, 0x110a, 0x110b, 0x110c, 0x110d, 0x110f, 0x1206, 0x1209, 0x120a, 0x120b, 0x120c, 0x120d, 0x120f, 0x1305, 0x1307, 0x1309, 0x130a, 0x130b, 0x130c, 0x130d, 0x130f, 0x1409, 0x140a, 0x140b, 0x140c, 0x140f, 0x1509, 0x150a, 0x150b, 0x150f, 0x1609, 0x160b, 0x170a};
const unsigned int icon_data_heatup[] PROGMEM = {0x0309, 0x030f, 0x0408, 0x0409, 0x040e, 0x040f, 0x0507, 0x0508, 0x0509, 0x050d, 0x050e, 0x050f, 0x0606, 0x0607, 0x0608, 0x0609, 0x060c, 0x060d, 0x060e, 0x060f, 0x0705, 0x0706, 0x0707, 0x0708, 0x0709, 0x070b, 0x070c, 0x070d, 0x070e, 0x070f, 0x0806, 0x0807, 0x0808, 0x0809, 0x080c, 0x080d, 0x080e, 0x080f, 0x0907, 0x0908, 0x0909, 0x090d, 0x090e, 0x090f, 0x0a08, 0x0a09, 0x0a0e, 0x0a0f, 0x0b09, 0x0b0f, 0x0e0d, 0x0f0e, 0x100c, 0x100f, 0x1104, 0x1105, 0x1106, 0x1107, 0x1108, 0x1109, 0x110a, 0x110d, 0x110f, 0x1204, 0x1205, 0x1206, 0x1207, 0x1208, 0x1209, 0x120a, 0x120b, 0x120d, 0x120f, 0x1304, 0x1305, 0x1306, 0x1307, 0x1308, 0x1309, 0x130a, 0x130d, 0x130f, 0x140c, 0x140f, 0x150e, 0x160d};
const unsigned int icon_data_pump[] PROGMEM = {0x0007, 0x0103, 0x0104, 0x0106, 0x0107, 0x0202, 0x0205, 0x0206, 0x0207, 0x0301, 0x0303, 0x0306, 0x0307, 0x0401, 0x0404, 0x0406, 0x0407, 0x0501, 0x0502, 0x0505, 0x0506, 0x0507, 0x0601, 0x0602, 0x0603, 0x0604, 0x0606, 0x0607, 0x0701, 0x0702, 0x0707};
const unsigned int icon_data_welcome[] PROGMEM = {0x0b20, 0x0b21, 0x0b22, 0x0b23, 0x0b24, 0x0c20, 0x0c22, 0x0c24, 0x0d20, 0x0d24, 0x1021, 0x1024, 0x1120, 0x1122, 0x1124, 0x1220, 0x1222, 0x1224, 0x1320, 0x1323, 0x1428, 0x1429, 0x142a, 0x1527, 0x152b, 0x160d, 0x160e, 0x160f, 0x1610, 0x1611, 0x1612, 0x1613, 0x1620, 0x1621, 0x1622, 0x1623, 0x1624, 0x1627, 0x162b, 0x170b, 0x170c, 0x170d, 0x170e, 0x170f, 0x1710, 0x1711, 0x1712, 0x1713, 0x1714, 0x1715, 0x1720, 0x1722, 0x172a, 0x172b, 0x1809, 0x180a, 0x180b, 0x180c, 0x1814, 0x1815, 0x1816, 0x1817, 0x1820, 0x1822, 0x1908, 0x1909, 0x190a, 0x1916, 0x1917, 0x1918, 0x1921, 0x1a07, 0x1a08, 0x1a18, 0x1a19, 0x1a27, 0x1a28, 0x1a29, 0x1a2a, 0x1b06, 0x1b07, 0x1b08, 0x1b18, 0x1b19, 0x1b1a, 0x1b2b, 0x1c06, 0x1c07, 0x1c19, 0x1c1a, 0x1c20, 0x1c21, 0x1c22, 0x1c23, 0x1c24, 0x1c2b, 0x1d05, 0x1d06, 0x1d1a, 0x1d1b, 0x1d20, 0x1d22, 0x1d27, 0x1d28, 0x1d29, 0x1d2a, 0x1e05, 0x1e06, 0x1e12, 0x1e13, 0x1e14, 0x1e15, 0x1e16, 0x1e17, 0x1e18, 0x1e19, 0x1e1a, 0x1e1b, 0x1e20, 0x1e22, 0x1e23, 0x1f05, 0x1f06, 0x1f0e, 0x1f0f, 0x1f10, 0x1f11, 0x1f12, 0x1f13, 0x1f14, 0x1f15, 0x1f16, 0x1f17, 0x1f18, 0x1f19, 0x1f1a, 0x1f1b, 0x1f21, 0x1f24, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200a, 0x200b, 0x200c, 0x200d, 0x200e, 0x200f, 0x2010, 0x2011, 0x2012, 0x201a, 0x201b, 0x2027, 0x2028, 0x2029, 0x202a, 0x202b, 0x2105, 0x2106, 0x2107, 0x2108, 0x2109, 0x210a, 0x210b, 0x210c, 0x210d, 0x210e, 0x211a, 0x211b, 0x2205, 0x2206, 0x221a, 0x221b, 0x2220, 0x2221, 0x2222, 0x2223, 0x2224, 0x2306, 0x2307, 0x2319, 0x231a, 0x2320, 0x2322, 0x2324, 0x2327, 0x2328, 0x2329, 0x232a, 0x232b, 0x2406, 0x2407, 0x2408, 0x2418, 0x2419, 0x241a, 0x2420, 0x2424, 0x2427, 0x242b, 0x2507, 0x2508, 0x2518, 0x2519, 0x2527, 0x252b, 0x2608, 0x2609, 0x260a, 0x2616, 0x2617, 0x2618, 0x2628, 0x2629, 0x262a, 0x2709, 0x270a, 0x270b, 0x270c, 0x2714, 0x2715, 0x2716, 0x2717, 0x2721, 0x2724, 0x280b, 0x280c, 0x280d, 0x280e, 0x280f, 0x2810, 0x2811, 0x2812, 0x2813, 0x2814, 0x2815, 0x2820, 0x2822, 0x2824, 0x290d, 0x290e, 0x290f, 0x2910, 0x2911, 0x2912, 0x2913, 0x2920, 0x2922, 0x2924, 0x2927, 0x2928, 0x2929, 0x292a, 0x292b, 0x2a20, 0x2a23, 0x2a27, 0x2a29, 0x2a2b, 0x2b27, 0x2b2b, 0x2d21, 0x2d24, 0x2e20, 0x2e22, 0x2e24, 0x2f20, 0x2f22, 0x2f24, 0x3020, 0x3023, 0x3321, 0x3322, 0x3323, 0x3420, 0x3424, 0x3520, 0x3524, 0x3621, 0x3622, 0x3623};
const unsigned int icon_data_toocold[] PROGMEM = {0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x040d, 0x040e, 0x040f, 0x0502, 0x0504, 0x0506, 0x0508, 0x050a, 0x050c, 0x0510, 0x0602, 0x0604, 0x0606, 0x0608, 0x060a, 0x060c, 0x060f, 0x0610, 0x0611, 0x0702, 0x070f, 0x0710, 0x0711, 0x0802, 0x0804, 0x0805, 0x0806, 0x0807, 0x0808, 0x0809, 0x080a, 0x080b, 0x080c, 0x080d, 0x080e, 0x080f, 0x0810, 0x0811, 0x0902, 0x0910, 0x0a02, 0x0a03, 0x0a04, 0x0a05, 0x0a06, 0x0a07, 0x0a08, 0x0a09, 0x0a0a, 0x0a0b, 0x0a0c, 0x0a0d, 0x0a0e, 0x0a0f, 0x0e0b, 0x0e0c, 0x0e11, 0x0f0c, 0x0f0d, 0x0f11, 0x100d, 0x100e, 0x1011, 0x1103, 0x1105, 0x1107, 0x1108, 0x1109, 0x110a, 0x110b, 0x110c, 0x110d, 0x110e, 0x110f, 0x1111, 0x1202, 0x1204, 0x1206, 0x1207, 0x1208, 0x1209, 0x120a, 0x120b, 0x120c, 0x120d, 0x120e, 0x120f, 0x1211, 0x130d, 0x130e, 0x1311, 0x140c, 0x140d, 0x1411, 0x150b, 0x150c, 0x1511};
const unsigned int icon_data_toohot[] PROGMEM = {0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x040d, 0x040e, 0x040f, 0x0502, 0x0504, 0x0506, 0x0508, 0x050a, 0x050c, 0x0510, 0x0602, 0x0604, 0x0606, 0x0608, 0x060a, 0x060c, 0x060f, 0x0610, 0x0611, 0x0702, 0x070f, 0x0710, 0x0711, 0x0802, 0x0804, 0x0805, 0x0806, 0x0807, 0x0808, 0x0809, 0x080a, 0x080b, 0x080c, 0x080d, 0x080e, 0x080f, 0x0810, 0x0811, 0x0902, 0x0910, 0x0a02, 0x0a03, 0x0a04, 0x0a05, 0x0a06, 0x0a07, 0x0a08, 0x0a09, 0x0a0a, 0x0a0b, 0x0a0c, 0x0a0d, 0x0a0e, 0x0a0f, 0x0e02, 0x0e07, 0x0e08, 0x0f02, 0x0f06, 0x0f07, 0x1002, 0x1005, 0x1006, 0x1102, 0x1104, 0x1105, 0x1106, 0x1107, 0x1108, 0x1109, 0x110a, 0x110b, 0x110c, 0x110e, 0x1110, 0x1202, 0x1204, 0x1205, 0x1206, 0x1207, 0x1208, 0x1209, 0x120a, 0x120b, 0x120c, 0x120d, 0x120f, 0x1211, 0x1302, 0x1305, 0x1306, 0x1402, 0x1406, 0x1407, 0x1502, 0x1507, 0x1508};
const unsigned int icon_data_steam[] PROGMEM = {0x0510, 0x0611, 0x070f, 0x0712, 0x080f, 0x0812, 0x090d, 0x0910, 0x0913, 0x0a0d, 0x0a10, 0x0a13, 0x0b01, 0x0b02, 0x0b03, 0x0b04, 0x0b05, 0x0b06, 0x0b07, 0x0b08, 0x0b09, 0x0b0d, 0x0b10, 0x0b13, 0x0c01, 0x0c02, 0x0c03, 0x0c04, 0x0c05, 0x0c06, 0x0c07, 0x0c08, 0x0c09, 0x0c0a, 0x0c0d, 0x0c10, 0x0c13, 0x0d01, 0x0d02, 0x0d03, 0x0d04, 0x0d05, 0x0d06, 0x0d07, 0x0d08, 0x0d09, 0x0d0d, 0x0d10, 0x0d13, 0x0e0d, 0x0e10, 0x0e13, 0x0f0d, 0x0f10, 0x0f13, 0x100f, 0x1012, 0x110f, 0x1112, 0x1211, 0x1310};


/**
 * Initializing the 1ms timer
 */
void setupTimer()
{
  /* disable all interrupts */
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 16000/64;                       /* compare match register 16MHz/64/1000Hz */
  TCCR1B |= (1 << WGM12);                 /* CTC mode */
  TCCR1B |= (1 << CS11) | (1 << CS10);    /* 64 prescaler */
  TIMSK1 |= (1 << OCIE1A);                /* enable timer compare interrupt */
  interrupts();                           /* enable all interrupts */
}


/**
 * Timer ISR
 */
ISR(TIMER1_COMPA_vect)
{
  int i;
  for(i = 0; i < MAX_COUNTERS; i++)
  {
    /* Check if the callback function is set */
    if(counters[i].callback != 0 && counters[i].elapsed != 1)
    {
      /* Increment the counter */
      counters[i].count++;

      /* Counter has reached the interval, set elapsed flag to 1,
         so the callback can be executed */
      if(counters[i].count > counters[i].interval)
      {
        counters[i].count = 0;
        counters[i].elapsed = 1;
      }
    }
  }
}

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


/**
 * Increments the shot counter. If the count is greater than 99 reset the counter
 */
void incrementShots(unsigned int *shot_count)
{
  (*shot_count)++;
  
  if(*shot_count > 99)
  {
    *shot_count = 0;
  }
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


/*
 * This function is called in the main loop. All timer callbacks with elapsed
 * flag set to 1 will be executed
 */
void executeCounters(CURRENT_STATE *state)
{
  int i;
  for(i = 0; i < MAX_COUNTERS; i++)
  {
    if(counters[i].elapsed && counters[i].callback != 0)
    {
      counters[i].elapsed = 0;
      counters[i].callback(state);
    }
  }
}

void copy_range(RANGE *from, RANGE *to)
{
  to->min = from->min;
  to->max = from->max; 
}

void setupConfig()
{
  /* Min temperature */
  config.min = 1;
  
  /* If the temperature is below cold start, the boiler is cold */
  config.warmup.min = 30.0;
  /* Lowest temperature edge, show the hedt up screen */
  config.warmup.max = 80.0;
  
  /* Lowest point of the espresso thermostat */
  config.espresso.min = 80.0;
  /* Highest point of the espresso thermostat */
  config.espresso.max = 105.0;

  /* Lowest point of the steam thermostat */
  config.steam.min = 150.0;
  /* Highest point of the steam thermostat */
  config.steam.max = 250.0;
  
  /* Max temperature of the boiler */
  config.max = 270;

  /* Minimal shot time */
  config.shot_min_time = 15;

  /* Delay in seconds before the brew counter starts counting */
  config.brew_counter_delay = 2;

  /* Temperature delta threshold */
  config.trend_delta = 0.5;
}

void drawIcon(ICON *icon, unsigned int x, unsigned int y)
{
  int i;
  
  for(i = 0; i < icon->count; i++)
  {
    unsigned int icon_data = pgm_read_word_near(icon->pixels + i);
    int ix = (icon_data >> 8) & 0x00FF;
    int iy = icon_data & 0x00FF;
    uView.pixel(x + ix, y + iy);
  }
}

void drawTopBar(CURRENT_STATE *state)
{
  POINT top_left, bottom_left, bottom_right;
  
  char buffer[] = "1:23:45";
  
  top_left.x = 0;
  top_left.y = 0;

  bottom_left.x = top_left.x;
  bottom_left.y = top_left.y + uView.getFontHeight();

  bottom_right.x = bottom_left.x + SCREEN_WIDTH;
  bottom_right.y = bottom_left.y;

  uView.line(bottom_left.x, bottom_left.y + 1, bottom_right.x, bottom_left.y + 1);

  uView.setCursor(bottom_left.x, bottom_left.y - uView.getFontHeight());

  formatTime(&state->run_time, buffer);

  /* Draw the time */
  uView.print(buffer);

  /* If the pump is on, show the symbol. Otherwise show the shot counter */
  if(state->pump)
  {
    /* Let the icon blink when waiting for the brew counter delay  */
    if((state->brew_counter_delay > 0 && state->blink_counter < 2)
        || state->brew_counter_delay == 0)
    {
      drawIcon(&icons[SCREEN_BREW], SCREEN_WIDTH - 8, 0);
    }
  }
  else
  {
    /* Draw the shot counter */
    if(state->shot_count > 0)
    {
      sprintf(buffer, "%02d", state->shot_count);
      uView.setCursor(SCREEN_WIDTH - uView.getFontWidth() * 2 - 1, bottom_left.y - uView.getFontHeight());
      uView.print(buffer);
    }
  }
}

void drawBottomBar(unsigned int from, unsigned int to, float percentage)
{
  POINT top_left, top_right, from_left, from_right, to_left, to_right;
  char buffer[5];

  top_left.x = 0;
  top_left.y = SCREEN_HEIGHT - uView.getFontHeight() - 1;

  top_right.x = top_left.x + SCREEN_WIDTH;
  top_right.y = top_left.y;

  uView.line(top_left.x, top_left.y - 1, top_right.x, top_left.y - 1);

  sprintf(buffer, "%d", from);

  from_left.x = 0;
  from_left.y = top_left.y + 2;
  from_right.x = (uView.getFontWidth() + 1) * getInt16PrintLen(from);
  from_right.y = from_left.y;

  /* Draw the "from" value */
  uView.setCursor(from_left.x, from_left.y);
  uView.print(buffer);

  sprintf(buffer, "%d", to);

  to_left.x = SCREEN_WIDTH - (uView.getFontWidth() + 1) * getInt16PrintLen(to) + 1;
  to_left.y = from_left.y;
  to_right.x = SCREEN_WIDTH;
  to_right.y = from_left.y;

  uView.setCursor(to_left.x, to_left.y);
  uView.print(buffer);

  int left = from_right.x + 1;
  int width = to_left.x - from_right.x - 4;
  int height = uView.getFontHeight() - 1;

  if(percentage > 1)
  {
    percentage = 1;
  }

  if(percentage < 0)
  {
    percentage = 0;
  }

  uView.rect(left, from_right.y, width + 1, height);
  
  uView.line(left + (int)(width * percentage), from_right.y, left + (int)(width * percentage), from_right.y + height);

  /* Fill the rect's each second pixel */
  for(int x = 0; x < (int)(width * percentage); x += 2)
  {
    for(int y = 0; y < height; y += 2)
    {
      uView.pixel(left + x, from_right.y + y);
    }
  }

  /* Draw black pixels to make the rect's corners look round */
  uView.setColor(BLACK);
  uView.pixel(left, from_right.y);
  uView.pixel(left + width, from_right.y);
  uView.pixel(left, from_right.y + height - 1);
  uView.pixel(left + width, from_right.y + height - 1);
  uView.setColor(WHITE);
}

/* Initializing the display */
void setupDisplay()
{
  uView.begin();
  uView.clear(PAGE);
  uView.display();
}

void setupState()
{
  current_state.error = ERROR_NONE;
  current_state.screen = SCREEN_WELCOME;
  current_state.shot_count = 0;
  initTime(&current_state.run_time);
  initTime(&current_state.brew_time);
  current_state.welcome_counter = WELCOME_SCREEN_DELAY;
}

void setupSerial()
{
  Serial.begin(115200);
}

void updateClock(CURRENT_STATE *state)
{
  incrementSeconds(&state->run_time);

  if(state->shot_state == SHOT_BREWING)
  {
    /* Starts counting the brew time only when the delay is over */
    if(state->brew_counter_delay > 0)
    {
      state->brew_counter_delay--;
    }
    else
    {
      incrementSeconds(&state->brew_time);
    }
  }

  /* Count down the welcome counter */
  if(state->welcome_counter > 0)
  {
    state->welcome_counter--;
  }
}

void updateAuxCounters(CURRENT_STATE *state)
{
  state->blink_counter++;
  
  if(state->blink_counter >= 4)
  {
    state->blink_counter = 0;
  }
}

void setupCounters()
{
  int i;
  for(i = 0; i < MAX_COUNTERS; i++)
  {
    counters[i].count = 0;
    counters[i].interval = 0;
    counters[i].callback = 0;
    counters[i].elapsed = 0;
  }

  i = 0;
  
  initCounter(&counters[0], 500, updateCurrentTemperature);
  initCounter(&counters[1], 500, selectScreen);
  initCounter(&counters[2], 1000, updateClock);
  initCounter(&counters[3], 250, updateAuxCounters);
  initCounter(&counters[4], 250, updateDisplay);
  initCounter(&counters[5], 50, updatePumpState);
}

void setupPins()
{
  pinMode(PIN_PUMP_MEAS, INPUT);
  pinMode(A1, INPUT);
}


void setupIcons()
{
  int i;
  for(i = 0; i < MAX_ICONS; i++)
  {
    icons[i].count = 0;
  }

  SET_ICON(SCREEN_ESPRESSO, icon_data_espresso_mug);
  SET_ICON(SCREEN_WARMUP, icon_data_warmup);
  SET_ICON(SCREEN_HEATUP, icon_data_heatup);
  SET_ICON(SCREEN_BREW, icon_data_pump);
  SET_ICON(SCREEN_WELCOME, icon_data_welcome);
  SET_ICON(SCREEN_STEAM, icon_data_steam);
  SET_ICON(SCREEN_TOOCOLD, icon_data_toocold);
  SET_ICON(SCREEN_TOOHOT, icon_data_toohot);
}

void setup()
{
  setupIcons();
  setupPins();
  setupSerial();
  setupConfig();
  setupState();
  setupDisplay();
  setupCounters();
  setupTimer();
}


/***
 * Draws an icon in the left part of the screen
 */
void drawMainIcon(CURRENT_STATE *state)
{
  POINT origin;
  
  unsigned char icon_height = 20;
  unsigned char icon_width = 26;
  
  origin.x = SCREEN_WIDTH / 4 - icon_width / 2;
  origin.y = SCREEN_HEIGHT / 2 - icon_height / 2;

  drawIcon(&icons[state->screen], origin.x, origin.y);
}


/***
 * Draws the brew counter where the main icon should be
 */
void drawBrewCounter(CURRENT_STATE *state)
{
  POINT origin;
  char buff[3];
  int old_font;
  
  origin.x = SCREEN_WIDTH / 4;
  origin.y = SCREEN_HEIGHT / 2 + 2;

  old_font = uView.getFontType();
  uView.setFontType(1);

  uView.setCursor(origin.x - uView.getFontWidth(), origin.y - uView.getFontHeight() / 2);
  sprintf(buff, "%02d", state->brew_time.seconds);

  /* Draw the brew counter with black color on white brackground */
  uView.setColor(WHITE);
  uView.rectFill(origin.x - uView.getFontWidth() - 3, origin.y - uView.getFontHeight() / 2 - 2, uView.getFontWidth() * 2 + 5, uView.getFontHeight());
  uView.setColor(BLACK);
  uView.print(buff);

  /* Font fix: erasing two lines overlapping the background */
  uView.lineH(origin.x - uView.getFontWidth() - 3, origin.y + uView.getFontHeight() / 2 - 2, uView.getFontWidth() * 2 + 5);
  uView.lineH(origin.x - uView.getFontWidth() - 3, origin.y + uView.getFontHeight() / 2 - 1, uView.getFontWidth() * 2 + 5);
  
  uView.setColor(WHITE);

  uView.setFontType(old_font);
}

void drawTrendArrow(unsigned char rising, unsigned int y_origin)
{
  POINT pos;
  unsigned int height = 5;
  int y;
  
  pos.y += y_origin + (rising ? (-1 * (height) - 3) : (uView.getFontHeight()) + 3);
  
  for(y = 0; y < height; y++)
  {
    if(rising)
    {
      uView.line(SCREEN_WIDTH/4*3 - y, pos.y + y, SCREEN_WIDTH/4*3 + y, pos.y + y);
    }
    else
    {
      uView.line(SCREEN_WIDTH/4*3 - (height - y - 1), pos.y + y, SCREEN_WIDTH/4*3 + (height - y - 1), pos.y + y);
    }
  }
}


/***
 * Function for outputting the data to the display 
 */
void updateDisplay(CURRENT_STATE *state)
{
  char temp[6];

  uView.clear(PAGE);

  if(state->screen == SCREEN_ERROR)
  {
    uView.setCursor(0, 0);
    switch(state->error)
    {
      case ERROR_SENSOR:
        uView.print("Sensor\nerror!");
        break;
      default:
        uView.print("Unknown\nerror!");
    }
  }
  else if(state->screen == SCREEN_WELCOME)
  {
    drawIcon(&icons[state->screen], 0, 0);
  }
  else
  {
    /* Display the run time and shot counter */
    drawTopBar(state);
  
    /* Calculate the percentage of the current temperature within the range  */
    float percentage = (state->temperature_fast - state->range.min)/(state->range.max - state->range.min);
  
    /* Display the current temperature range */
    drawBottomBar((unsigned int)state->range.min, (unsigned int)state->range.max, percentage);
  
    /* Display the current temperature */
    int temp_width = (getInt16PrintLen((int)(state->temperature_fast)) + 2)*(uView.getFontWidth() + 1) + 1;
    int temp_y = SCREEN_HEIGHT / 2 - uView.getFontHeight() / 2;
  
    /* Move the cursor for drawing the temperature to the 3/4 of the screen */
    uView.setCursor(SCREEN_WIDTH/2 + (SCREEN_WIDTH/4 - temp_width/2), temp_y);
  
    /* Print out the temperature with one decimal place */
    dtostrf(state->temperature_fast, 3, 1, temp);
    uView.print(temp);
  
    /* Draw the trend arrow to indicate the rising or falling temperature */
    if(state->blink_counter < 1)
    {
      if(state->temp_trend > 0)
      {
        drawTrendArrow(1, temp_y);
      } 
      else if(state->temp_trend < 0)
      {
        drawTrendArrow(0, temp_y);
      }
    }
    
    /* The screen for brewing should show a counter instead of an icon */
    if(state->screen == SCREEN_BREW)
    {
      drawBrewCounter(state);
    }
    else
    {
      drawMainIcon(state);
    }
  }
  
  uView.display();
}


/**
 * Hook for the screen state machine. When screens change, this function gets called
 * and can be used to determin the transaction (e.g. switch from screen A to screen B)
 */
void changeScreen(CURRENT_STATE *state, SCREEN_TYPE new_screen)
{
  /* Every screen different from "brew" means the "shot is in idle" */
  if(new_screen != SCREEN_BREW)
  {
    /* Check the time and increment the shot counter when going 
       from "brew" to some different state (like "espresso") */
    if(state->screen == SCREEN_BREW)
    {
      /* If the brew time exeeds a specific period, a shot was (probably) done */
      if(timeInSeconds(&state->brew_time) >= config.shot_min_time)
      {
        /* Reset the time */
        initTime(&state->brew_time);
        
        /* Increment number of shots */
        incrementShots(&state->shot_count);
      }
    }
    
    state->shot_state = SHOT_IDLE;
  }
  else
  {
    /* Going from "espresso" state to the "brew" state, so the shot started */
    if(state->screen == SCREEN_ESPRESSO && new_screen == SCREEN_BREW)
    {
      /* Reset the brew time */
      initTime(&state->brew_time);
      state->brew_counter_delay = config.brew_counter_delay;
      state->shot_state = SHOT_BREWING;
    }
    
    
  }
  
  state->screen = new_screen;
}


/**
 * This functions selects a screen depending on measured values like temperature
 * or pump state
 */
void selectScreen(CURRENT_STATE *state)
{
  /* Showing an error :( */
  if(state->error != ERROR_NONE)
  {
    changeScreen(state, SCREEN_ERROR);
    return;
  }

  if(state->welcome_counter > 0)
  {
    changeScreen(state, SCREEN_WELCOME);
    return;
  }

  /* Not warming up, too cold, may be sensor fault? */
  if(state->temperature < config.warmup.min)
  {
    state->range.min = config.min;
    state->range.max = config.warmup.min;
    changeScreen(state, SCREEN_TOOCOLD);
    return;
  }

  /* Warm up phase */
  if(state->temperature >= config.warmup.min &&
     state->temperature < config.warmup.max)
  {
    copy_range(&config.warmup, &state->range);
    changeScreen(state, SCREEN_WARMUP);
    return;
  }

  /* Making an espresso */
  if(state->temperature >= config.espresso.min &&
     state->temperature < config.espresso.max)
  {
    copy_range(&config.espresso, &state->range);

    if(state->pump)
    {
      changeScreen(state, SCREEN_BREW);
    }
    else
    {
      changeScreen(state, SCREEN_ESPRESSO);
    }

    return;
  }

  /* Heating up for steam */
  if(state->temperature >= config.espresso.max &&
     state->temperature < config.steam.min)
  {
    state->range.min = config.espresso.max;
    state->range.max = config.steam.min;
    changeScreen(state, SCREEN_HEATUP);
    return;
  }

  /* Making steam */
  if(state->temperature >= config.steam.min &&
     state->temperature < config.max)
  {
    copy_range(&config.steam, &state->range);
    changeScreen(state, SCREEN_STEAM);
    return;
  }

  /* Getting too hot! */
  if(state->temperature >= config.max)
  {
    copy_range(&config.steam, &state->range);
    changeScreen(state, SCREEN_TOOHOT);
    return;
  }
}


/**
 * Function for reading the pump state
 */
void updatePumpState(CURRENT_STATE *state)
{
  int current_value = analogRead(PIN_PUMP_MEAS);

  if(current_value > 512)
  {
    state->pump = 1;
  }
  else
  {
    state->pump = 0;
  }
}

/**
 * Function for reading the actual temperature value
 */
void updateCurrentTemperature(CURRENT_STATE *state)
{
  /* TODO: filter temperature values so they don't update to quickly */

  
  double t = (analogRead(A1)/1024.0*300.0);

  if(t < 5.0)
  {
    t = thermocouple.readCelsius();
  }
  

  /* If the temperature is NaN or below one degree, set the error flag */
  if(isnan(t) || t < config.min)
  {
    state->error = ERROR_SENSOR;
    t = 0;
  }
  else
  {
    state->error = ERROR_NONE;
  }
  
  state->temperature_fast = t;

  /* Initialize the last value */
  if(state->temperature < config.warmup.min)
  {
    state->temperature = state->temperature_fast;
  }

  double delta_t = state->temperature_fast - state->last_temperature;

  if(delta_t > config.trend_delta)
  {
    state->temp_trend = 1;
  }
  else if(delta_t < -1 * config.trend_delta)
  {
    state->temp_trend = -1;
  }
  else
  {
    state->temp_trend = 0;
  }

  state->last_temperature = state->temperature_fast;

  /* Build the filtered value */
  state->temperature = state->temperature_fast;//(state->temperature_fast + state->temperature) / 2;
}

void printOutDebug(CURRENT_STATE *state)
{
  Serial.print("Error:      ");
  Serial.println(state->error);
  
  Serial.print("Temp:       ");
  Serial.println(state->temperature);

  Serial.print("Fast Temp:  ");
  Serial.println(state->temperature_fast);

  Serial.print("Pump:       ");
  Serial.println(state->pump);
  
  Serial.print("Screen:     ");
  Serial.println(state->screen);

  Serial.print("Range:      ");
  Serial.print(state->range.min);
  Serial.print(" - ");
  Serial.println(state->range.max);

  Serial.println("--------------------");
}

void loop()
{
  executeCounters(&current_state);
  
  if(current_state.error)
  {
    delay(1000);
  }
  else
  {
    delay(1);
  }
}

