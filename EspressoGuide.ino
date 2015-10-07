#include "espresso_guide.h"

SCREEN_TYPE current_screen = SCREEN_WELCOME;

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

/* Initializing the display */
void setupDisplay()
{
  
}

void setup()
{
  setupDisplay();
  setupTimer();
}

/* Function for outputting the data to the display */
void updateDisplay()
{
  switch(current_screen)
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


void loop()
{
  
  updateDisplay();
}
