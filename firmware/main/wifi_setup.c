#include "wifi_setup.h"

//private variables
// EventGroupHandle_t wifi_setup_event_group_handle;

// private function declaration
static void WIFI_SETUP_Event_Handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/**
 * @brief Is called whenever any connection related WiFi event happens
 */
static void WIFI_SETUP_Event_Handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    const char *TAG = __func__;
    if (event_base == WIFI_EVENT)
    {
        if (event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }
        // stuff to do if we disconnect
        if (event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            wifi_event_sta_disconnected_t *disconnected_event_ptr = (wifi_event_sta_disconnected_t *)event_data;
            ESP_LOGI(TAG, "Disconnected!");
            esp_wifi_connect();
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP : %s", ip4addr_ntoa(&event->ip_info.ip));
        // xEventGroupSetBits(wifi_setup_event_group_handle, WIFI_CONNECTED_BIT);
    }
}

esp_err_t WIFI_SETUP_Init(void)
{
    const char *TAG = __func__;

    //creating the event group
    // wifi_setup_event_group_handle = xEventGroupCreate();

    // starting TCPIP adapter
    tcpip_adapter_init();
    // starting event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // everything default
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // registering event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_SETUP_Event_Handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WIFI_SETUP_Event_Handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &WIFI_SETUP_Event_Handler, NULL));

    // set these in the kconfig
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASS},
    };

    // if the password has any length, it means there's some sort of security so we set the type as WPA2
    // change this if needed
    if (strlen((char *)wifi_config.sta.password))
    {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    // setting station mode and starting WiFi
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // EventBits_t wifi_event_bits = xEventGroupWaitBits(wifi_setup_event_group_handle, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    ESP_LOGI(TAG, "wifi_init_sta finished.");
    return ESP_OK;
}