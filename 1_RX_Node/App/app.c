/* official c headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
/* avr libc includes */
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
/* private includes */
#include "Globals.h"
#include "App.h"
#include "ADC.h"
#include "DS3231.h"
#include "Sonar.h"
#include "UART.h"
#include "Wakeups.h"
#include "EEPROM.h"

uint32_t min_voltages[2] = {819,553};

int8_t  ambient_temp;
uint32_t batteries[2];
char 	health[3] ={'x','x','x'};
uint8_t distance;

uint8_t mute = 0;
void app_init(void)
{
	wdt_reset();
	wdt_disable();

	/* Clear all Interupts */
	cli();
	DDRC = 0x00;
	/* Enable all four status LEDs */
	DDRB  = (1<<LED_3_PIN)|(1<<LED_4_PIN)|(1<<5);
	DDRD = (1<<LED_1_PIN)|(1<<LED_2_PIN);

	PORTB = (1<<LED_3_PIN)|(1<<LED_4_PIN);
	PORTD = (1<<LED_1_PIN)|(1<<LED_2_PIN);	;

	PCICR  =(1<<PCIE1)|(1<<PCIE2);
	PCMSK1 =(1<<PCINT11);
	PCMSK2 =(1<<PCINT18);


	/* Initialize Core */
	adc_init();
	wakeups_init();
	ds3231_init();
	sonar_init();
	uart_init();
	sei();

	printf("Init finished");

}

void app_reset(void)
{
	/* Reset RTC */
	RST_PORT |= (1<<RST_PIN);
	/* Dump EEPROM */
	clear_eeprom();
	/* Force an Hardware Reset */
	wdt_enable(WDTO_2S);
	_delay_ms(3000);
}

/* Configure Alarms over UART */
void app_config(void)
{
char input[20];
uint8_t index= 0;
char c = 'a';

printf("Enter new Alarm,end menue by typing 'z'\r\n");

/* If end character is send close configuration */
while(c != 'z')
	{
		/* Wait for new chars */
		if(!(UCSR0A & (1<<RXC0)));
		
		else
		{
			c =UDR0;
			input[index] = c;
			index++;
			
		}
		
	}
	input[index-1]= 0;
	//uart_tx(input);
	/* Save new Alarm to EEPROM */
	save_timestamp(input);
	printf("Leaving Configuration menue");
	
}

/* Collect continuosly data */
void op_normal(void)
{
	/* Get temperature */
	ambient_temp = rtc_get_temp();
	/* Measure TOF + Distance */
	send_pulse();
	_delay_ms(60);
	distance = calc_distance(ambient_temp);
	/* Check batteries and give alarm if nec. */
	batteries[0] = single_conversion(0);
	batteries[1] = single_conversion(2);
	//send_package(batteries,distance);
	//debug(ambient_temp,batteries,health, distance);

}

/*Enter Idle Mode */
void op_stop(void)
{
	printf("Idling MCU");
	set_sleep_mode(SLEEP_MODE_IDLE);  
	sleep_enable();  
	sleep_bod_disable();
  	sleep_mode(); 
	/* Leave Sleep Mode */
	sleep_disable();  
	printf("Back to standard run mode");
}

/*Enter Deep Sleep*/
void op_sleep(void)
{
printf("entering sleep mode");
/* Set All Pins to Inputs */ 
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
		
	DDRB |= 0x00;
	DDRC |= 0x00;
	DDRB |= 0x00;
	/* Disable the ADC */	
	ADCSRA ^= (1<<7);
	/* Disable BOD and put CPU to Sleep */
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
	sleep_enable();  
	sleep_bod_disable();
  	sleep_mode(); 
	/* Leave Sleep Mode */
	sleep_disable();
	_delay_ms(1000);  
	printf("Back up runnning"); 


}
void op_button(void)
{
app_reset();
}

void op_mute(void)
{
mute = !mute;
}

void debug(int8_t ambient_temp, uint32_t *batteries,char *health,uint8_t distance)
{
printf("Ambient Temperature: %i, Distance: %i, Main Supply Voltage %i, Backup Supply Voltage: %i",(int)ambient_temp,(int)distance,(int)batteries[0],(int)batteries[1]);
}

void send_package(uint32_t *batteries, uint8_t distance)
{
int i=0;
if(mute)
	{
	for(i=0; i<2;i++)
		{
			if(batteries[i] < min_voltages[i])
				printf("Low Voltage Alert for Battery %i \r\n",i);
		}
	}
printf("Distance:%i",(int)distance);
}

