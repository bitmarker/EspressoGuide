#include "espresso_guide.h"

static TEMP_CONFIG temp_config;
static CURRENT_STATE state;
  
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
  state.error = ERROR_NONE;
  state.screen = SCREEN_WELCOME;
}

void setup()
{
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

void showScreenSteam()
{
  
}

void showScreenError()
{
  
}

void showScreenPump()
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
    case SCREEN_STEAM:
      showScreenSteam();
      break;
    case SCREEN_ERROR:
      showScreenError();
      break;
    case SCREEN_PUMP:
      showScreenPump();
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
    state->screen = SCREEN_ESPRESSO;
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
}

void updateCurrentTemperature(CURRENT_STATE *state)
{
  state->temperature = 25.0;
}

void loop()
{
  /* Read the sensor */
  updateCurrentTemperature(&state);
  
  /* Check which screen should be displayed */
  selectScreen(&state);
  
  /* Draw the display */
  updateDisplay(&state);

  /* Wait a bit... */
  delay(500);
}

