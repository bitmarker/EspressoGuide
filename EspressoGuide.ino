#include "espresso_guide.h"
#include <MicroView.h>

CONFIG config;
CURRENT_STATE current_state;

#define MAX_COUNTERS 5
ACTION_COUNTER counters[MAX_COUNTERS];

int SCREEN_WIDTH = uView.getLCDWidth();
int SCREEN_HEIGHT = uView.getLCDHeight();

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
  config.trend_delta = 0.1;
}

void drawTopBar(TIME *t, unsigned int shot_count)
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

  formatTime(t, buffer);

  /* Draw the time */
  uView.print(buffer);

  /* Draw the shot counter */
  if(shot_count > 0)
  {
    sprintf(buffer, "%02d", shot_count);
    uView.setCursor(SCREEN_WIDTH - uView.getFontWidth() * 2 - 1, bottom_left.y - uView.getFontHeight());
    uView.print(buffer);
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
}

void setup()
{
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
  unsigned char icon_width = 20;
  
  origin.x = SCREEN_WIDTH / 4 - icon_width / 2;
  origin.y = SCREEN_HEIGHT / 2 - icon_height / 2;

  uView.rect(origin.x, origin.y, icon_width, icon_height);

  uView.setCursor(origin.x + 2, origin.y + 2);
  uView.print(state->screen);
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

  /* Display the run time and shot counter */
  drawTopBar(&state->run_time, state->shot_count);

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
    /* TODO: draw the counter */
  }
  else
  {
    drawMainIcon(state);
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
    
    /* Check the time and increment the shot counter when going 
       from "brew" to some different state (like "espresso") */
    if(state->screen == SCREEN_BREW && new_screen != SCREEN_BREW)
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

  /* Not warming up, too cold, may be sensor fault? */
  if(state->temperature < config.warmup.min)
  {
    state->range.min = 0;
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
 * Function for reading the actual temperature value
 */
void updateCurrentTemperature(CURRENT_STATE *state)
{
  /* TODO: filter temperature values so they don't update to quickly */

  
  static double t = 0;//Serial.parseFloat();

  if(t > 250)
  {
    t = 0;
    //state->temperature_fast = t;
  }

  t++;

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
}

