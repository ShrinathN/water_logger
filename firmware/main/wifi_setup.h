#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include "main.h"

//macros
#define WIFI_CONNECTED_BIT BIT0

//public functions

/**
 * @brief Connects to WiFi network
 */
esp_err_t WIFI_SETUP_Init(void);

#endif