#include "stdint.h"

void BT_to_HMI(char rxData[]);
uint8_t calc_own_charge(uint16_t battery_voltage);
uint32_t backlight_pwm(uint16_t sensor1,uint16_t sensor2 ,uint16_t sensor3);

	