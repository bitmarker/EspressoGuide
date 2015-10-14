#include "espresso_guide.h"
#include <MicroView.h>

TEMP_CONFIG temp_config;
CURRENT_STATE current_state;


int SCREEN_WIDTH = uView.getLCDWidth();
int SCREEN_HEIGHT = uView.getLCDHeight();

#define MAX_COUNTERS 5
ACTION_COUNTER counters[MAX_COUNTERS];


/* Initializing the 1ms timer */
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

/* Timer ISR  */
ISR(TIMER1_COMPA_vect)
{
  int i;
  for(i = 0; i < MAX_COUNTERS; i++)
  {
    /* Check if the callback function is set */
    if(counters[i].callback != 0)
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

void initTime(TIME *t)
{
  t->seconds = 0;
  t->hours = 0;
  t->minutes = 0;
}

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

void incrementShots(unsigned int *shot_count)
{
  (*shot_count)++;
  
  if(*shot_count > 99)
  {
    *shot_count = 0;
  }
}

void initCounter(ACTION_COUNTER *counter, unsigned int interval, ACTION_COUNTER_CALLBACK callback)
{
  counter->elapsed = 0;
  counter->interval = interval;
  counter->count = 0;
  counter->callback = callback;
}

void executeCounters(CURRENT_STATE *state)
{
  int i;
  for(i = 0; i < MAX_COUNTERS; i++)
  {
    if(counters[i].elapsed && counters[i].callback != 0)
    {
      counters[i].callback(state);
      counters[i].elapsed = 0;
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
  temp_config.warmup.min = 30.0;
  /* Lowest temperature edge, show the hedt up screen */
  temp_config.warmup.max = 80.0;
  
  /* Lowest point of the espresso thermostat */
  temp_config.espresso.min = 80.0;
  /* Highest point of the espresso thermostat */
  temp_config.espresso.max = 105.0;

  /* Lowest point of the steam thermostat */
  temp_config.steam.min = 150.0;
  /* Highest point of the steam thermostat */
  temp_config.steam.max = 250.0;
  
  /* Max temperature of the boiler */
  temp_config.max = 270;
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

  uView.line(bottom_left.x, bottom_left.y, bottom_right.x, bottom_left.y);

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

  uView.line(top_left.x, top_left.y, top_right.x, top_left.y);

  sprintf(buffer, "%d", from);

  from_left.x = 0;
  from_left.y = top_left.y + 2;
  from_right.x = uView.getFontWidth() * getInt16PrintLen(from);
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

  int left = from_right.x + 3;
  int width = to_left.x - from_right.x - 5;
  int height = uView.getFontHeight() - 1;

  if(percentage > 1)
  {
    percentage = 1;
  }

  if(percentage < 0)
  {
    percentage = 0;
  }

  uView.rect(left, from_right.y, width, height);
  uView.rectFill(left, from_right.y, (int)(width * percentage), height);
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

  if(state->pump)
  {
    incrementSeconds(&state->brew_time);
  }

  
  incrementShots(&state->shot_count);
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
  
  initCounter(&counters[i++], 500, updateCurrentTemperature);
  initCounter(&counters[i++], 500, selectScreen);
  initCounter(&counters[i++], 1000, updateClock);
  initCounter(&counters[i++], 500, updateDisplay);
  
  //initCounter(&counters[i++], 1000, printOutDebug);
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

void showScreenWelcome()
{
  
}

void showScreenWarmUp()
{
  
}

void showScreenEspresso()
{
  
}

void showScreenHeatUp()
{
  
}

void showScreenSteam()
{
  
}

void showScreenError()
{
  
}

void showScreenPump()
{
  
}

void showScreenOverRange(SCREEN_TYPE screen)
{
  
}

void drawTrendArrow(unsigned char rising, unsigned int y_origin)
{
  POINT pos;
  unsigned int height = 5;
  int y;
  
  pos.y += y_origin + (rising ? (-1 * (height) - 2) : (uView.getFontHeight()) + 2);
  
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
  uView.print((int)(state->temperature_fast));
  uView.print(".");
  uView.print((int)((state->temperature_fast - (int)(state->temperature_fast)) * 10));

  drawTrendArrow(0, temp_y);
  drawTrendArrow(1, temp_y);
  
  /* TODO: change this */
  uView.setCursor(0, SCREEN_HEIGHT / 2 - uView.getFontHeight() / 2);
  uView.print(state->screen);


  /* Display screen specific information */
  switch(state->screen)
  {
    case SCREEN_WELCOME:
      showScreenWelcome();
      break;
    case SCREEN_WARMUP:
      showScreenWarmUp();
      break;
    case SCREEN_ESPRESSO:
      showScreenEspresso();
      break;
    case SCREEN_HEATUP:
      showScreenHeatUp();
      break;
    case SCREEN_STEAM:
      showScreenSteam();
      break;
    case SCREEN_ERROR:
      showScreenError();
      break;
    case SCREEN_BREW:
      showScreenPump();
      break;
    case SCREEN_TOOCOLD:
    case SCREEN_TOOHOT:
      showScreenOverRange(state->screen);
      break;
    default:
      break;
  }

  uView.display();
}

void changeScreen(CURRENT_STATE *state, SCREEN_TYPE new_screen)
{
  
  
  state->screen = new_screen;
}

void selectScreen(CURRENT_STATE *state)
{
  /*
  if(state->screen != SCREEN_BREW)
  {
    state->shot_state = SHOT_IDLE;
  }
  */
  
  /* Showing an error :( */
  if(state->error != ERROR_NONE)
  {
    changeScreen(state, SCREEN_ERROR);
    return;
  }

  /* Not warming up, too cold, may be sensor fault? */
  if(state->temperature < temp_config.warmup.min)
  {
    state->range.min = 0;
    state->range.max = temp_config.warmup.min;
    changeScreen(state, SCREEN_TOOCOLD);
    return;
  }

  /* Warm up phase */
  if(state->temperature >= temp_config.warmup.min &&
     state->temperature < temp_config.warmup.max)
  {
    copy_range(&temp_config.warmup, &state->range);
    changeScreen(state, SCREEN_WARMUP);
    return;
  }

  /* Making an espresso */
  if(state->temperature >= temp_config.espresso.min &&
     state->temperature < temp_config.espresso.max)
  {
    copy_range(&temp_config.espresso, &state->range);

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
  if(state->temperature >= temp_config.espresso.max &&
     state->temperature < temp_config.steam.min)
  {
    state->range.min = temp_config.espresso.max;
    state->range.max = temp_config.steam.min;
    state->screen = SCREEN_HEATUP;
    return;
  }

  /* Making steam */
  if(state->temperature >= temp_config.steam.min &&
     state->temperature < temp_config.max)
  {
    copy_range(&temp_config.steam, &state->range);
    state->screen = SCREEN_STEAM;
    return;
  }

  /* Getting too hot! */
  if(state->temperature >= temp_config.max)
  {
    copy_range(&temp_config.steam, &state->range);
    state->screen = SCREEN_TOOHOT;
    return;
  }
}

void updateCurrentTemperature(CURRENT_STATE *state)
{
  /* TODO: filter temperature values so they don't update to quickly */

  
  double t = Serial.parseFloat();

  if(t > 0)
  {
    state->temperature_fast = t;
  }

  /* Initialize the last value */
  if(state->temperature < temp_config.warmup.min)
  {
    state->temperature = state->temperature_fast;
  }

  state->temperature = state->temperature_fast;

  /* Build the filtered value */
  //state->temperature = (state->temperature_fast + state->temperature) / 2;
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
  delay(1);
}

