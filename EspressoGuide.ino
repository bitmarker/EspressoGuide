#include "espresso_guide.h"

static TEMP_CONFIG temp_config;
static CURRENT_STATE current_state;

#define MAX_COUNTERS 5
static ACTION_COUNTER counters[MAX_COUNTERS];


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
    counters[i].count++;
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
  temp_config.warmup.max = 70.0;
  
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

/* Initializing the display */
void setupDisplay()
{
  
}

void setupState()
{
  current_state.error = ERROR_NONE;
  current_state.screen = SCREEN_WELCOME;
}

void setupSerial()
{
  Serial.begin(115200);
}

void setup()
{
  setupSerial();
  setupConfig();
  setupState();
  setupDisplay();
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

/* Function for outputting the data to the display */
void updateDisplay(CURRENT_STATE *state)
{
  /* Display basic information, equal for every screen */

  /* TODO: Add code here */

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
    case SCREEN_PUMP:
      showScreenPump();
      break;
    case SCREEN_TOOCOLD:
    case SCREEN_TOOHOT:
      showScreenOverRange(state->screen);
      break;
    default:
      break;
  }
}

void selectScreen(CURRENT_STATE *state)
{
  /* Showing an error :( */
  if(state->error != ERROR_NONE)
  {
    state->screen = SCREEN_ERROR;
    return;
  }

  /* Not warming up, too cold, may be sensor fault? */
  if(state->temperature < temp_config.warmup.min)
  {
    copy_range(&temp_config.warmup, &state->range);
    state->screen = SCREEN_TOOCOLD;
    return;
  }

  /* Warm up phase */
  if(state->temperature >= temp_config.warmup.min &&
     state->temperature < temp_config.warmup.max)
  {
    copy_range(&temp_config.warmup, &state->range);
    state->screen = SCREEN_WARMUP;
    return;
  }

  /* Making an espresso */
  if(state->temperature >= temp_config.espresso.min &&
     state->temperature < temp_config.espresso.max)
  {
    copy_range(&temp_config.espresso, &state->range);

    if(state->pump)
    {
      state->screen = SCREEN_PUMP;
    }
    else
    {
      state->screen = SCREEN_ESPRESSO;
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
     state->temperature < temp_config.steam.max)
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

  /* Build the filtered value */
  state->temperature = (state->temperature_fast + state->temperature) / 2;
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
  /* Read the sensor */
  updateCurrentTemperature(&current_state);
  
  /* Check which screen should be displayed */
  selectScreen(&current_state);
  
  /* Draw the display */
  updateDisplay(&current_state);

  printOutDebug(&current_state);

  /* Wait a bit... */
  delay(500);
}

