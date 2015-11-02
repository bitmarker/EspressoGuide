/*
  EspressoGuide
  Copyright (C) 2015 Leonid Lezner

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utils.h"
#include "espresso_guide.h"
#include <MicroView.h>
#include <TSIC.h>
#include "imagedata.h"

CURRENT_STATE current_state;

/* Define constants */
#define PIN_PUMP_MEAS         A0
#define PIN_DI                3
#define PIN_VCC               2

#define MAX_ALLOWED_ERRORS    5
#define WELCOME_SCREEN_DELAY  3 /* seconds */
#define SLOPE_DELTA           0.03
#define FONT_CHAR_HEIGHT      20 /* pixels */
#define MAX_COUNTERS          6
#define DEVICE_DESCRIPTION    "EspressoGuide v0.1.0"
#define PUMP_THRESHOLD        15

/* Define global variables */
ACTION_COUNTER counters[MAX_COUNTERS];
uint16_t SCREEN_WIDTH = uView.getLCDWidth();
uint16_t SCREEN_HEIGHT = uView.getLCDHeight();
TSIC mainTempSensor(PIN_DI, PIN_VCC);

void setupPins()
{
  pinMode(PIN_PUMP_MEAS, INPUT);
}

byte measurePumpState()
{
  uint16_t pump_val = 0;
  pump_val = analogRead(PIN_PUMP_MEAS);
  return pump_val > PUMP_THRESHOLD;
}

double measureMainTemperature()
{
  uint16_t temp_raw;
  double temp;
  uint8_t res;
  
  noInterrupts();
  res = mainTempSensor.getTemperature(&temp_raw);
  interrupts();
  
  if(res)
  {
    temp = mainTempSensor.calc_Celsius(&temp_raw);
    return temp;
  }

  return NAN;
}

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
  OCR1A = 16000 / 64;                     /* compare match register 16MHz/64/1000Hz */
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
  uint16_t i;
  for (i = 0; i < MAX_COUNTERS; i++)
  {
    /* Check if the callback function is set */
    if (counters[i].callback != 0 && counters[i].elapsed != 1)
    {
      /* Increment the counter */
      counters[i].count++;

      /* Counter has reached the interval, set elapsed flag to 1,
         so the callback can be executed */
      if (counters[i].count > counters[i].interval)
      {
        counters[i].count = 0;
        counters[i].elapsed = 1;
      }
    }
  }
}

/*
 * This function is called in the main loop. All timer callbacks with elapsed
 * flag set to 1 will be executed
 */
void executeCounters(CURRENT_STATE *state)
{
  uint16_t i;
  for (i = 0; i < MAX_COUNTERS; i++)
  {
    if (counters[i].elapsed && counters[i].callback != 0)
    {
      counters[i].elapsed = 0;
      counters[i].callback(state);
    }
  }
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
  current_state.error_counter = 0;
  current_state.error = ERROR_NONE;
  current_state.screen = SCREEN_WELCOME;
  initTime(&current_state.run_time);
  initTime(&current_state.brew_time);
  current_state.welcome_counter = WELCOME_SCREEN_DELAY;
  initMeasData(&current_state.temp_data);
}

void setupSerial()
{
  Serial.begin(115200);
  Serial.println(DEVICE_DESCRIPTION);
}

void updateClock(CURRENT_STATE *state)
{
  incrementSeconds(&state->run_time);

  if (state->shot_state == SHOT_BREWING)
  {
    incrementSeconds(&state->brew_time);
  }

  /* Count down the welcome counter */
  if (state->welcome_counter > 0)
  {
    state->welcome_counter--;
  }
}

void updateAuxCounters(CURRENT_STATE *state)
{
  state->blink_counter++;

  if (state->blink_counter >= 4)
  {
    state->blink_counter = 0;
  }
}

void setupCounters()
{
  int i;
  for (i = 0; i < MAX_COUNTERS; i++)
  {
    counters[i].count = 0;
    counters[i].interval = 0;
    counters[i].callback = 0;
    counters[i].elapsed = 0;
  }

  i = 0;

  initCounter(&counters[0], 500, updateCurrentTemperature);
  initCounter(&counters[1], 250, selectScreen);
  initCounter(&counters[2], 1000, updateClock);
  initCounter(&counters[3], 250, updateAuxCounters);
  initCounter(&counters[4], 250, updateDisplay);
  initCounter(&counters[5], 250, updatePumpState);
}


void setup()
{
  setupPins();
  setupSerial();
  setupState();
  setupDisplay();
  setupCounters();
  setupTimer();
}

void drawIcon(const uint16_t *data, uint16_t len, uint16_t x, uint16_t y)
{
  uint16_t i;

  for (i = 0; i < len; i++)
  {
    uint16_t icon_data = pgm_read_word_near(data + i);
    int ix = (icon_data >> 8) & 0x00FF;
    int iy = icon_data & 0x00FF;
    uView.pixel(x + ix, y + iy);
  }
}


void drawErrorScreen(CURRENT_STATE *state)
{
  uView.setCursor(0, 0);
  switch (state->error)
  {
    case ERROR_SENSOR:
      uView.print("Sensor\nerror!");
      break;
    case ERROR_NONE:
      break;
    default:
      uView.print("Unknown\nerror!");
  }
}

void drawWelcomeScreen(CURRENT_STATE *state)
{
  DRAW_ICON(welcome, 0, 0);
}

void drawDigit(uint8_t number, uint16_t x, uint16_t y)
{
  switch (number)
  {
    case 0: DRAW_ICON(zero, x, y); break;
    case 1: DRAW_ICON(one, x, y); break;
    case 2: DRAW_ICON(two, x, y); break;
    case 3: DRAW_ICON(three, x, y); break;
    case 4: DRAW_ICON(four, x, y); break;
    case 5: DRAW_ICON(five, x, y); break;
    case 6: DRAW_ICON(six, x, y); break;
    case 7: DRAW_ICON(seven, x, y); break;
    case 8: DRAW_ICON(eight, x, y); break;
    case 9: DRAW_ICON(nine, x, y); break;
  }
}

uint16_t getDigitWidth(uint8_t number)
{
  switch (number)
  {
    case 0: return ICON_WIDTH(zero);
    case 1: return ICON_WIDTH(one);
    case 2: return ICON_WIDTH(two);
    case 3: return ICON_WIDTH(three);
    case 4: return ICON_WIDTH(four);
    case 5: return ICON_WIDTH(five);
    case 6: return ICON_WIDTH(six);
    case 7: return ICON_WIDTH(seven);
    case 8: return ICON_WIDTH(eight);
    case 9: return ICON_WIDTH(nine);
  }
  return 0;
}

uint16_t getNumberWidth(uint16_t number)
{
  uint8_t digits[4];
  uint8_t count = getNumberDigits(number, digits);
  uint16_t width = 0;

  for (int i = 0; i < count; i++)
  {
    width += (getDigitWidth(digits[i]) + 2);
  }

  return width - 2;
}

void printNumber(uint16_t number, POINT *origin)
{
  uint16_t x = origin->x;
  uint8_t digits[4];
  uint8_t count = getNumberDigits(number, digits);

  for (int i = 0; i < count; i++)
  {
    drawDigit(digits[i], x, origin->y);
    x += (getDigitWidth(digits[i]) + 2);
  }
}

void dottedLineH(uint16_t x, int16_t y, int16_t width)
{
  for (int i = 0; i < width; i+=2)
  {
    uView.pixel(x + i, y);
  }
}

void dottedLineV(uint16_t x, int16_t y, int16_t height)
{
  for (int i = 0; i < height; i+=2)
  {
    uView.pixel(x, y + i);
  }
}

void drawTrendArrow(byte rising, uint16_t y_center, uint16_t distance, uint8_t icon_type)
{
  uint16_t height = 5;
  int x, y;
  uint8_t icon_height = (icon_type == 1) ? 16 : 8;
  uint8_t icon_width = (icon_type == 1) ? 8 : 8;

  if(rising)
  {
    x = SCREEN_WIDTH / 2 + (distance) / 2;
    y = y_center - icon_height/2;

    if(icon_type == 1)
    {
      DRAW_ICON(trend_up_big, x, y);
    }
    else
    {
      DRAW_ICON(trend_up_small, x, y);
    }
  }
  else
  {
    x = SCREEN_WIDTH / 2 - (distance) / 2 - icon_width;
    y = y_center - icon_height/2;

    if(icon_type == 1)
    {
      DRAW_ICON(trend_down_big, x, y);
    }
    else
    {
      DRAW_ICON(trend_down_small, x, y);
    }
  }
}

void drawSubInformation(uint8_t y, const char *buff, CURRENT_STATE *state, uint16_t main_width)
{
  y += 3;

  uint16_t width = strlen(buff) * (uView.getFontWidth() + 1) - 1;

  uint16_t x = SCREEN_WIDTH / 2 - width / 2;

  uint16_t lineWidth = width;

  if(main_width > width)
  {
    lineWidth = main_width;
  }
  else
  {
    lineWidth = width;
  }

  uView.lineH(SCREEN_WIDTH / 2 - (lineWidth + 6) / 2, y - 4, lineWidth + 6);
  
  uView.setCursor(x, y);
  uView.print(buff);

  if(state->screen == SCREEN_BREW)
  {
    drawTempTrend(state, y + uView.getFontHeight() / 2, strlen(buff) * (uView.getFontWidth() + 1) + 2, 2);
  }
}


void drawMainNumber(uint16_t number, CURRENT_STATE *state, uint16_t *offset, uint16_t *width)
{
  POINT origin;
  int height = FONT_CHAR_HEIGHT;
  *width = getNumberWidth(number);
  origin.y = 7;
  origin.x = SCREEN_WIDTH / 2 - *width / 2;
  printNumber(number, &origin);
  
  if(state->screen == SCREEN_IDLE)
  {
    drawTempTrend(state, origin.y + height/2, *width + 1, 1);
  }
  
  *offset = origin.y + height + 4;
}

void drawTempTrend(CURRENT_STATE *state, uint16_t y_center, uint16_t distance, uint8_t icon_type)
{
  /* Draw the trend arrow to indicate the rising or falling temperature */
  if (state->blink_counter < 2)
  {
    /* Temperature is rising */
    if (state->temp_trend > 0)
    {
      drawTrendArrow(1, y_center, distance, icon_type);
    }
    else if (state->temp_trend < 0) /* Temperature is falling */
    {
      drawTrendArrow(0, y_center, distance, icon_type);
    }
  }
}


void drawIdleScreen(CURRENT_STATE *state)
{
  char buff[10];
  uint16_t y, width;
  uint16_t main_number = state->screen == SCREEN_BREW ? state->brew_time.seconds : round(state->temperature);
  drawMainNumber(main_number, state, &y, &width);
  formatTime(&state->run_time, buff);
  drawSubInformation(y, buff, state, width);
}


void drawBrewScreen(CURRENT_STATE *state)
{
  char buff[10];
  uint16_t y, width;
  
  drawMainNumber(state->brew_time.seconds, state, &y, &width);
  
  dottedLineH(0, 0, SCREEN_WIDTH);
  dottedLineH(1, SCREEN_HEIGHT - 1, SCREEN_WIDTH);
  dottedLineV(0, 0, SCREEN_HEIGHT);
  dottedLineV(SCREEN_WIDTH - 1, 1, SCREEN_HEIGHT);
  
  sprintf(buff, "%02d", round(state->temperature));
  drawSubInformation(y, buff, state, width);
}


/***
 * Function for outputting the data to the display
 */
void updateDisplay(CURRENT_STATE *state)
{
  uView.clear(PAGE);

  switch (state->screen)
  {
    case SCREEN_ERROR:
      drawErrorScreen(state);
      break;
    case SCREEN_WELCOME:
      drawWelcomeScreen(state);
      break;
    case SCREEN_IDLE:
      drawIdleScreen(state);
      break;
    case SCREEN_BREW:
      drawBrewScreen(state);
      break;
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
  if (new_screen != SCREEN_BREW)
  {
    if (state->screen == SCREEN_BREW)
    {
      /* Reset the time */
      initTime(&state->brew_time);
    }

    state->shot_state = SHOT_IDLE;
  }
  else
  {
    /* Going from any state to the "brew" state, so the shot started */
    if (state->screen != SCREEN_BREW && new_screen == SCREEN_BREW)
    {
      /* Reset the brew time */
      initTime(&state->brew_time);
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
  if (state->error != ERROR_NONE)
  {
    changeScreen(state, SCREEN_ERROR);
    return;
  }

  if (state->welcome_counter > 0)
  {
    changeScreen(state, SCREEN_WELCOME);
    return;
  }
  else
  {
    if (state->pump)
    {
      changeScreen(state, SCREEN_BREW);
    }
    else
    {
      changeScreen(state, SCREEN_IDLE);
    }
  }
}

/**
 * Function for reading the pump state
 */
void updatePumpState(CURRENT_STATE *state)
{
  /* TODO: add some filtering here... */
  state->pump = measurePumpState();
}

/**
 * Function for reading the actual temperature value
 */
void updateCurrentTemperature(CURRENT_STATE *state)
{
  double t = measureMainTemperature();

  /* If the temperature is NaN or zero, set the error flag */
  if (isnan(t) || t < 0.1 || t > 250)
  {
    if(state->error_counter <= MAX_ALLOWED_ERRORS)
    {
      state->error_counter++;
    }
    else
    {
      state->error_counter = 0;
      state->error = ERROR_SENSOR;
    }
    
    t = 0;
    return;
  }
  else
  {
    state->error_counter = 0;
    state->error = ERROR_NONE;
    state->temperature_fast = t;
  }

  /* Add the value to the last measurements buffer and calculate the linearization */
  addNextValue(&state->temp_data, state->temperature_fast);

  /* Build the filtered value */
  state->temperature = state->temperature_fast;

  /* Set the trend value corresponding to the slope */
  if (state->temp_data.slope > SLOPE_DELTA)
  {
    state->temp_trend = 1;
  }
  else if (state->temp_data.slope < -1 * SLOPE_DELTA)
  {
    state->temp_trend = -1;
  }
  else
  {
    state->temp_trend = 0;
  }
}

void loop()
{
  executeCounters(&current_state);
  delay(1);
}

