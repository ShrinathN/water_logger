#include "main.h"
#include "wifi_setup.h"
#include "HX711.h"
#include "application.h"

// private functions
static void blinky_task(void *arg);
static void blink_led(void);
static void start_blinky_task(void);

// private variables
static WaterDrinkEvent water_drink_events[100] = ZERO_ARRAY;
static uint8_t water_drink_events_counter = 0;

/**
 * Runs every 10 minutes, blinks the LED 5 times to remind to drink water. It also uploads data to server
 */
static void blinky_task(void *arg)
{
    const char *TAG = __func__;
    ESP_LOGI(TAG, "Running blinky task!");
    while (1)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            blink_led();
        }

        if (water_drink_events_counter > 0)
        {
            if (APPLICATION_Send_data_to_server(&water_drink_events, sizeof(WaterDrinkEvent) * (water_drink_events_counter)) == ESP_OK)
            {
                // if everything was sent successfully we reset memory
                water_drink_events_counter = 0;
                memset(&water_drink_events, 0, sizeof(water_drink_events));
            }
        }
        vTaskDelay((60 * 10 * 1000) / portTICK_RATE_MS);
    }
}

static uint32_t get_stable_reading()
{
    const char *TAG = __func__;
    uint32_t a = 0;
    uint32_t b = 0;
    while (1)
    {
        a = HX711_Get_Valid_Weight();
        ESP_LOGI(TAG, "a = %ug", a);
        vTaskDelay(500 / portTICK_RATE_MS);
        b = HX711_Get_Valid_Weight();
        ESP_LOGI(TAG, "b = %ug", b);
        vTaskDelay(500 / portTICK_RATE_MS);

        if (a == b && a != 0)
        {
            return a;
        }
    }
}

static void blink_led()
{
    gpio_set_level(PIN_LED, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_LED, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
}

static void start_blinky_task(void)
{
    xTaskCreate(blinky_task, "blinky_task", 1024, NULL, 0, NULL);
}

static void gpio_init(void)
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

static void process_bottle_down_event(int32_t old_weight, int32_t new_weight)
{

    int32_t water_weight = old_weight - new_weight;

    // this means new weight is higher, which means the bottle was filled
    // we will assume that all remaining water was drunk and the bottle was then refilled
    if (water_weight < 0)
    {
        water_weight = old_weight - 235;
    }
    // this means the new weight is lower, which means some water was drank
    // we calculate this and we're done (do we nothing)
    else
    {
    }

    // adding event to log
    time(&water_drink_events[water_drink_events_counter].timestamp);
    water_drink_events[water_drink_events_counter++].water_drank = water_weight;
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

    // getting current time
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    // logic variables
    BottleState current_state = BOTTLE_OFF;
    BottleState previous_state = BOTTLE_OFF;
    DrinkState drinking_state = WAITING;

    int32_t zero_weight = get_stable_reading();
    int32_t bottle_weight = 0;

    bool first_run = true;

    // starting blinky task
    start_blinky_task();

    while (1)
    {
        // getting current weight
        int32_t weight_g = get_stable_reading() - zero_weight;
        ESP_LOGI(TAG, "stable reading = %u", weight_g);

        // setting current state as old state
        previous_state = current_state;

        // if the weight is less than 20g, the bottle is off
        if (weight_g < 20)
        {
            current_state = BOTTLE_OFF;
        }
        if (weight_g >= 20)
        {
            current_state = BOTTLE_ON;
            // bottle_weight = weight_g;
        }

        if (previous_state == BOTTLE_OFF)
        {
            // this means bottle was recently placed
            // we shall wait until both states are BOTTLE_ON to consider it a bottle place event, which signals the end of a sip
            if (current_state == BOTTLE_ON)
            {
                if (drinking_state == WAITING)
                {

                    // if the bottle was placed for the first time
                    if (first_run)
                    {
                        blink_led();
                        blink_led();
                        bottle_weight = weight_g;
                        first_run = false;
                    }
                }
            }

            if (current_state == BOTTLE_OFF)
            {
                //
                if (drinking_state == WAITING)
                {
                }

                // this means drinking is still in progress
                // do nothing
                if (drinking_state == DRINKING)
                {
                    drinking_state = DRINKING;
                }
            }
        }
        // previous_state == BOTTLE_ON
        else
        {
            // meaning bottle is still on
            if (current_state == BOTTLE_ON)
            {
                // this means the drink has finally ended
                // we can minus the old weight from the current weight and get the water drank
                if (drinking_state == DRINKING)
                {
                    ESP_LOGI(TAG, "Bottle placed back again!");
                    drinking_state = WAITING;
                    process_bottle_down_event(bottle_weight, weight_g);
                    blink_led();
                    bottle_weight = weight_g;
                }
                if (drinking_state == WAITING)
                {
                    bottle_weight = weight_g;
                }
            }

            // bottle off now
            //  this means drinking is in progress
            if (current_state == BOTTLE_OFF)
            {
                if (drinking_state == WAITING)
                {
                    drinking_state = DRINKING;
                    blink_led();
                    ESP_LOGI(TAG, "Bottle off detected!");
                }
            }
        }

        // // test routine
        // uint32_t weight_g = 33;
        // char format[100] = "Time = %s\nWeight = %u\n%u";
        // char buffer[100];
        // time_t now = 0;
        // time(&now);
        // sprintf(buffer, format, ctime(&now), weight_g, sizeof(WaterDrinkEvent));
        // APPLICATION_Send_data_to_server((uint8_t *)buffer, (size_t *)strlen(buffer));

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
