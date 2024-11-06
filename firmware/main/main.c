#include "main.h"
#include "wifi_setup.h"
#include "HX711.h"

void gpio_init(void)
{
    gpio_config_t gp = ZERO_ARRAY;

    // GPIO0 - HX711_PD_SCK_PIN
    PIN_FUNC_SELECT(PERIPHS_GPIO_MUX_REG(PIN_HX711_CLK), GPIO_FUNC_SET(PIN_HX711_CLK));
    gp.intr_type = GPIO_INTR_DISABLE;
    gp.mode = GPIO_MODE_OUTPUT;
    gp.pin_bit_mask = BIT(PIN_HX711_CLK);
    gp.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gp.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gp);

    // GPIO2 - HX711_DT_PIN
    PIN_FUNC_SELECT(PERIPHS_GPIO_MUX_REG(PIN_HX711_DT), GPIO_FUNC_SET(PIN_HX711_DT));
    gp.mode = GPIO_MODE_INPUT;
    gp.pin_bit_mask = BIT(PIN_HX711_DT);
    gpio_config(&gp);

    // GPIO1 - LED
    PIN_FUNC_SELECT(PERIPHS_GPIO_MUX_REG(PIN_LED), GPIO_FUNC_SET(PIN_LED));
    gp.mode = GPIO_MODE_OUTPUT;
    gp.pin_bit_mask = BIT(PIN_LED);
    gpio_config(&gp);

    // // GPIO3 - Buzzer
    // PIN_FUNC_SELECT(PERIPHS_GPIO_MUX_REG(PIN_BUZZER), GPIO_FUNC_SET(PIN_BUZZER));
    // gp.mode = GPIO_MODE_OUTPUT;
    // gp.pin_bit_mask = BIT(PIN_BUZZER);
    // gpio_config(&gp);
}

uint32_t get_valid_weight()
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


void app_main()
{
    const char *TAG = __func__;

    // initialising GPIOs
    gpio_init();

    // starting code
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "Code running!");

    // connecting to WiFi network
    ESP_ERROR_CHECK(WIFI_SETUP_Init());

    while (1)
    {
        uint32_t w = get_valid_weight();
        ESP_LOGI(TAG, "Weight is %u", w);
        if(w > 450)
        {
            gpio_set_level(PIN_LED, 1);
        }
        else
        {
            gpio_set_level(PIN_LED, 0);
        }
        vTaskDelay(1000 / portTICK_RATE_MS);

    }
}
