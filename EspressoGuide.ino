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
#define WELCOME_SCREEN_DELAY  30

ACTION_COUNTER counters[MAX_COUNTERS];

ICON icons[MAX_ICONS];

int SCREEN_WIDTH = uView.getLCDWidth();
int SCREEN_HEIGHT = uView.getLCDHeight();

Adafruit_MAX31855 thermocouple(PIN_CLK, PIN_CS, PIN_DO);

const unsigned int icon_data_espresso_mug[] PROGMEM = {0x0611, 0x0708, 0x0709, 0x070a, 0x070b, 0x0711, 0x0801, 0x0805, 0x0808, 0x080a, 0x080b, 0x080c, 0x080d, 0x0811, 0x0902, 0x0904, 0x0906, 0x0908, 0x0909, 0x090a, 0x090c, 0x090d, 0x090e, 0x0911, 0x0a03, 0x0a08, 0x0a0a, 0x0a0b, 0x0a0c, 0x0a0e, 0x0a0f, 0x0a11, 0x0b08, 0x0b09, 0x0b0a, 0x0b0c, 0x0b0d, 0x0b0e, 0x0b0f, 0x0b11, 0x0c02, 0x0c05, 0x0c08, 0x0c0a, 0x0c0b, 0x0c0c, 0x0c0e, 0x0c0f, 0x0c11, 0x0d03, 0x0d04, 0x0d06, 0x0d08, 0x0d09, 0x0d0a, 0x0d0c, 0x0d0d, 0x0d0e, 0x0d0f, 0x0d11, 0x0e08, 0x0e0a, 0x0e0b, 0x0e0c, 0x0e0e, 0x0e0f, 0x0e11, 0x0f01, 0x0f05, 0x0f08, 0x0f09, 0x0f0a, 0x0f0b, 0x0f0c, 0x0f0d, 0x0f0e, 0x0f0f, 0x0f11, 0x1002, 0x1004, 0x1006, 0x1008, 0x1009, 0x100a, 0x100b, 0x100c, 0x100d, 0x100e, 0x1011, 0x1103, 0x1108, 0x1109, 0x110a, 0x110b, 0x110c, 0x110d, 0x1111, 0x1208, 0x1209, 0x120a, 0x120b, 0x120c, 0x1211, 0x1308, 0x130c, 0x1311, 0x1408, 0x140c, 0x1509, 0x150a, 0x150b};
const unsigned int icon_data_warmup[] PROGMEM = {0x0309, 0x030f, 0x0408, 0x0409, 0x040e, 0x040f, 0x0507, 0x0508, 0x0509, 0x050d, 0x050e, 0x050f, 0x0606, 0x0607, 0x0608, 0x0609, 0x060c, 0x060d, 0x060e, 0x060f, 0x0705, 0x0706, 0x0707, 0x0708, 0x0709, 0x070b, 0x070c, 0x070d, 0x070e, 0x070f, 0x0806, 0x0807, 0x0808, 0x0809, 0x080c, 0x080d, 0x080e, 0x080f, 0x0907, 0x0908, 0x0909, 0x090d, 0x090e, 0x090f, 0x0a08, 0x0a09, 0x0a0e, 0x0a0f, 0x0b09, 0x0b0f, 0x0e09, 0x0e0a, 0x0e0f, 0x0f06, 0x0f09, 0x0f0a, 0x0f0b, 0x0f0c, 0x0f0f, 0x1005, 0x1007, 0x1009, 0x100a, 0x100b, 0x100c, 0x100d, 0x100f, 0x1109, 0x110a, 0x110b, 0x110c, 0x110d, 0x110f, 0x1206, 0x1209, 0x120a, 0x120b, 0x120c, 0x120d, 0x120f, 0x1305, 0x1307, 0x1309, 0x130a, 0x130b, 0x130c, 0x130d, 0x130f, 0x1409, 0x140a, 0x140b, 0x140c, 0x140f, 0x1509, 0x150a, 0x150b, 0x150f, 0x1609, 0x160b, 0x170a};
const unsigned int icon_data_heatup[] PROGMEM = {0x0309, 0x030f, 0x0408, 0x0409, 0x040e, 0x040f, 0x0507, 0x0508, 0x0509, 0x050d, 0x050e, 0x050f, 0x0606, 0x0607, 0x0608, 0x0609, 0x060c, 0x060d, 0x060e, 0x060f, 0x0705, 0x0706, 0x0707, 0x0708, 0x0709, 0x070b, 0x070c, 0x070d, 0x070e, 0x070f, 0x0806, 0x0807, 0x0808, 0x0809, 0x080c, 0x080d, 0x080e, 0x080f, 0x0907, 0x0908, 0x0909, 0x090d, 0x090e, 0x090f, 0x0a08, 0x0a09, 0x0a0e, 0x0a0f, 0x0b09, 0x0b0f, 0x0e0d, 0x0f0e, 0x100c, 0x100f, 0x1104, 0x1105, 0x1106, 0x1107, 0x1108, 0x1109, 0x110a, 0x110d, 0x110f, 0x1204, 0x1205, 0x1206, 0x1207, 0x1208, 0x1209, 0x120a, 0x120b, 0x120d, 0x120f, 0x1304, 0x1305, 0x1306, 0x1307, 0x1308, 0x1309, 0x130a, 0x130d, 0x130f, 0x140c, 0x140f, 0x150e, 0x160d};
const unsigned int icon_data_pump[] PROGMEM = {0x0007, 0x0103, 0x0104, 0x0106, 0x0107, 0x0202, 0x0205, 0x0206, 0x0207, 0x0301, 0x0303, 0x0306, 0x0307, 0x0401, 0x0404, 0x0406, 0x0407, 0x0501, 0x0502, 0x0505, 0x0506, 0x0507, 0x0601, 0x0602, 0x0603, 0x0604, 0x0606, 0x0607, 0x0701, 0x0702, 0x0707};

// welcome
// toocold
// toohot
// steam


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
  config.trend_delta = 1;
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

int z = 0;

void drawMainIcon(CURRENT_STATE *state)
{
  POINT origin;
  
  unsigned char icon_height = 20;
  unsigned char icon_width = 26;
  
  origin.x = SCREEN_WIDTH / 4 - icon_width / 2;
  origin.y = SCREEN_HEIGHT / 2 - icon_height / 2;

  drawIcon(&icons[state->screen], origin.x, origin.y);
}

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

/* Function for outputting the data to the display */
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
    uView.setCursor(0, 0);
    uView.print("Espresso\nGuide!");
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

  
  double t = (analogRead(A1)/1024.0*250.0);

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

