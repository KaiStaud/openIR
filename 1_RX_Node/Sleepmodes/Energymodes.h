#include <avr/interrupt.h>

/* Extern Request Handler */
void Change_PowerMode(char c);

/* Initialize Wakeup Pin */
void wakeup_init(void);

/* Interrupt Request generated by RTC */
ISR(PCINT1_vect);
