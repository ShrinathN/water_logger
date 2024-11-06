#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"


//driver stuff
#include "driver/gpio.h"
#include "driver/soc.h"
#include "esp8266/gpio_register.h"
#include "esp8266/gpio_struct.h"
#include "esp8266/pin_mux_register.h"

//macros
#define ZERO_ARRAY {0,}
#define GPIO_FUNC_SET_NUM(X) FUNC_GPIO##X
#define GPIO_FUNC_SET(X) GPIO_FUNC_SET_NUM(X)

//config
#define PIN_HX711_CLK 0
#define PIN_HX711_DT 2
#define PIN_LED 3
// #define PIN_BUZZER 3

#define HX711_DT_PIN PIN_HX711_DT
#define HX711_PD_SCK_PIN PIN_HX711_CLK

#endif