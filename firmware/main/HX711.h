#ifndef HX711_H
#define HX711_H

// includes
#include "main.h"

// macros
#define HIGH 1
#define LOW 0

#define RETRY_TIMEOUT 1000000
#define HX711_PULSE_TIME 5
#define HX711_PULSE_TIME_US 20

#define HX711_ZERO 24173
#define HX711_300G 107049

#define HX711_GET_DOUT gpio_get_level(HX711_DT_PIN)
#define HX711_SET_SCK(X) gpio_set_level(HX711_PD_SCK_PIN, X)
#define HX711_DELAY_US(X) ets_delay_us(X)

// structs
typedef struct
{
	uint32_t reading_0; // gain 128, channel A
	uint32_t reading_1; // gain 32, channel B
	uint32_t reading_2; // gain 64, channel A
} HX711_Result;

// public functions
void HX711_Init(void);
bool HX711_Get_Readings(HX711_Result *ptr_to_result);
uint32_t HX711_Get_Weight();
uint32_t HX711_Get_Valid_Weight();
#endif