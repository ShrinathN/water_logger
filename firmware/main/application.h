#ifndef APPLICATION_H
#define APPLICATION_H

// includes
#include "main.h"

// enum
typedef enum
{
    BOTTLE_OFF,
    BOTTLE_ON
} BottleState;

typedef enum
{
    WAITING,
    DRINKING
}DrinkState;

// public functions
esp_err_t APPLICATION_Send_data_to_server(uint8_t *data_ptr, size_t data_size);

#endif