#include "HX711.h"

void HX711_Init(void)
{
	HX711_SET_SCK(LOW);
}

bool HX711_Get_Readings(HX711_Result *ptr_to_result)
{
	uint32_t retry_timeout_ctr = 0;
	ptr_to_result->reading_0 = 0;
	ptr_to_result->reading_1 = 0;
	ptr_to_result->reading_2 = 0;

	// getting first conversion
	// waiting until DOUT is high
	retry_timeout_ctr = 0;
	do
	{
		HX711_DELAY_US(1);
	} while (HX711_GET_DOUT && retry_timeout_ctr++ < RETRY_TIMEOUT);
	if (retry_timeout_ctr >= RETRY_TIMEOUT)
	{
		return false;
	}

	// sending 24 pulses on SCK while reading the DOUT level on falling edge
	for (uint32_t i = 0; i < 24; i++)
	{
		HX711_SET_SCK(HIGH);									// rising edge
		HX711_DELAY_US(HX711_PULSE_TIME_US);					// waiting for time
		HX711_SET_SCK(LOW);										// rising edge
		ptr_to_result->reading_0 |= HX711_GET_DOUT << (23 - i); // reading the bit
		HX711_DELAY_US(HX711_PULSE_TIME_US);					// waiting for time
	}

	// sending the 25th pulse
	HX711_SET_SCK(HIGH);				 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(LOW);					 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time

	// getting the second conversion
	// // waiting until DOUT is low
	// retry_timeout_ctr = 0;
	// do
	// {
	// 	HX711_DELAY_US(1);
	// } while (!HX711_GET_DOUT && retry_timeout_ctr++ < RETRY_TIMEOUT);
	// if (retry_timeout_ctr >= RETRY_TIMEOUT)
	// {
	// 	return false;
	// }

	// waiting until DOUT is high
	retry_timeout_ctr = 0;
	do
	{
		HX711_DELAY_US(1);
	} while (HX711_GET_DOUT && retry_timeout_ctr++ < RETRY_TIMEOUT);
	if (retry_timeout_ctr >= RETRY_TIMEOUT)
	{
		return false;
	}

	// sending 24 pulses on SCK while reading the DOUT level on falling edge
	for (uint32_t i = 0; i < 24; i++)
	{
		HX711_SET_SCK(HIGH);									// rising edge
		HX711_DELAY_US(HX711_PULSE_TIME_US);					// waiting for time
		HX711_SET_SCK(LOW);										// rising edge
		ptr_to_result->reading_1 |= HX711_GET_DOUT << (23 - i); // reading the bit
		HX711_DELAY_US(HX711_PULSE_TIME_US);					// waiting for time
	}

	// sending the 25, 26 pulse
	HX711_SET_SCK(HIGH);				 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(LOW);					 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(HIGH);				 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(LOW);					 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time

	// getting the third conversion
	// waiting until DOUT is low
	// retry_timeout_ctr = 0;
	// do
	// {
	// 	HX711_DELAY_US(1);
	// } while (!HX711_GET_DOUT && retry_timeout_ctr++ < RETRY_TIMEOUT);
	// if (retry_timeout_ctr >= RETRY_TIMEOUT)
	// {
	// 	return false;
	// }

	// waiting until DOUT is high
	retry_timeout_ctr = 0;
	do
	{
		HX711_DELAY_US(1);
	} while (HX711_GET_DOUT && retry_timeout_ctr++ < RETRY_TIMEOUT);
	if (retry_timeout_ctr >= RETRY_TIMEOUT)
	{
		return false;
	}

	// sending 24 pulses on SCK while reading the DOUT level on falling edge
	for (uint32_t i = 0; i < 24; i++)
	{
		HX711_SET_SCK(HIGH);									// rising edge
		HX711_DELAY_US(HX711_PULSE_TIME_US);					// waiting for time
		HX711_SET_SCK(LOW);										// rising edge
		ptr_to_result->reading_2 |= HX711_GET_DOUT << (23 - i); // reading the bit
		HX711_DELAY_US(HX711_PULSE_TIME_US);					// waiting for time
	}

	// sending the 25, 26 and 27 pulse
	HX711_SET_SCK(HIGH);				 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(LOW);					 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(HIGH);				 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(LOW);					 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(HIGH);				 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time
	HX711_SET_SCK(LOW);					 // rising edge
	HX711_DELAY_US(HX711_PULSE_TIME_US); // waiting for time

	return true;
}

static double HX711_adc_to_weight_g(int weight)
{
	return (2.55e-06 * weight) - 2.17e-03;
}

uint32_t HX711_Get_Weight()
{
	HX711_Result res = ZERO_ARRAY;
	bool x = HX711_Get_Readings(&res);
	res.reading_1 &= ~(0xff);
	res.reading_1 = (res.reading_1 & (1 << 23)) ? ((res.reading_1 & ~(1 << 23)) | (1 << 31)) : (res.reading_1 & ~(1 << 23));
	int data = (int)(*(&res.reading_1)) - HX711_ZERO;
	uint32_t data_to_return = (uint32_t)(HX711_adc_to_weight_g(data) * 1000);
	return data_to_return;
}

uint32_t HX711_Get_Valid_Weight()
{
	uint32_t to_return = HX711_Get_Weight();
	if (to_return < 15)
	{
		to_return = 0;
	}

	if (to_return > 2000)
	{
		to_return = 0;
	}

	return to_return;
}
