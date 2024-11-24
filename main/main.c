#include "lwip/sys.h"
#include "esp_wifi.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include <stdio.h>
#include "esp_event.h"
#include "lwip/err.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "http.c"
#include "mqtt.c"
// #include "soil_moisture_sensor.c"
#include "bh1750.h"
#include "esp_timer.h"
#include "dht11.h"
#include "GATTserver.c"

#define LED_GPIO GPIO_NUM_2                   // pin diody LED
#define TICKS(ms) ((ms) / portTICK_PERIOD_MS) // opóźnia zadanie o podaną liczbę ms

const char *ssid = "moje_nazwa"; // nazwa Wi-Fi (Tu podajecie swoje)
const char *pass = "1234567890"; // hasło
static bool isConnected = false;
static bool gotIP = false;

static void blink_led(void *arg)
{
    while (1)
    {
        if (!isConnected)
        {
            gpio_set_level(LED_GPIO, 1); // włącza diodę
            vTaskDelay(TICKS(500));
            gpio_set_level(LED_GPIO, 0); // wyłącza diodę
            vTaskDelay(TICKS(500));
        }
        else
        {
            gpio_set_level(LED_GPIO, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

static void handle_wifi_event(void *context, esp_event_base_t base_event, int32_t wifi_event_id, void *event_info)
{
    switch (wifi_event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("Łączenie do Wi-Fi....\n");
        break;

    case WIFI_EVENT_STA_CONNECTED:
        printf("Połączono do Wi-Fi\n");
        isConnected = true;
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        printf("Utracono połączenie do Wi-Fi\n");
        isConnected = false;
        gotIP = false;

        vTaskDelay(TICKS(2000));
        printf("Próba ponownego połączenia do Wi-Fi...\n");
        esp_wifi_connect();
        break;

    case IP_EVENT_STA_GOT_IP:
        printf("Wi-Fi uzyskało IP.\n");
        gotIP = true;
        break;

    default:
        printf("Nieznane zdarzenie Wi-Fi: %ld\n", wifi_event_id);
        break;
    }
}

void initialize_wifi()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, handle_wifi_event, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, handle_wifi_event, NULL);
}

void configure_wifi_credentials()
{
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
        }};

    snprintf((char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", ssid);
    snprintf((char *)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", pass);

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
}

void setup_wifi_station()
{
    initialize_wifi();
    configure_wifi_credentials();

    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);

    esp_wifi_connect();
    printf("Wi-Fi zainicjowane. SSID:%s  Hasło:%s\n", ssid, pass);
}

void make_request()
{
    bool requestSent = false;

    while (1)
    {
        if (isConnected && !requestSent)
        {
            printf("Wysyłanie zapytania HTTP GET...\n");
            char *response = malloc(MAX_HTTP_OUTPUT_BUFFER + 1);
            http_get_request("http://httpbin.org/get", response);
            // http_get_request("http://www.http2demo.io/");
            requestSent = true;

            if (response != NULL)
            {
                printf("Response: %s\n", response);
                free(response);
            }
            else
            {
                printf("Failed to get response\n");
            }
        }
        else
        {
            vTaskDelay(TICKS(1000));
        }
    }
}

void setup_mqtt()
{
    bool setUp = false;

    while (1)
    {
        if (gotIP && !setUp)
        {
            printf("Setting up MQTT...\n");
            mqtt_start("mqtt://10.42.0.1:1883");
            setUp = true;
        }
        else if (!gotIP && setUp)
        {
            setUp = false;
        }
        vTaskDelay(TICKS(1000));
    }
}

// void read_dht11_task(void *arg)
// {
//     DHT11_init(GPIO_NUM_4);

//     while (1)
//     {
//         struct dht11_reading reading = DHT11_read();
//         if (reading.status == DHT11_OK)
//         {
//             printf("Temperature: %d °C\n", reading.temperature);
//             printf("Humidity: %d %%\n", reading.humidity);
//         }
//         else
//         {
//             printf("Error reading DHT11 sensor. Status code: %d\n", reading.status);
//         }
//         vTaskDelay(TICKS(2000));
//     }
// }

// void app_main(void) {

//     nvs_flash_init();

//     esp_rom_gpio_pad_select_gpio(LED_GPIO);
//     gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
//     gpio_set_level(LED_GPIO, 0);

//     setup_wifi_station();
//     init_ble();
//     xTaskCreate(blink_led, "blink_led_task", 1024, NULL, 5, NULL);
//     // xTaskCreate(make_request, "make_request", 8192, NULL, 4, NULL);

// //     // xTaskCreate(setup_mqtt, "setup_mqtt", 8192, NULL, 4, NULL);
// //     // // xTaskCreate(readBH1750, "read_bh1750", 2048, NULL, 5, NULL);
//     xTaskCreate(read_dht11_task, "read_dht11_task", 2048, NULL, 5, NULL);

// //     // Żeby wypisywane były pomiary wilgotności gleby wystarczy odkomentować poniższy fragment:
// //     /*
// //     soil_moisture_sensor_init();

// //     while (1) {
// //         float moisture_level = soil_moisture_sensor_read();
// //         printf("Wilgotność gleby: %.2f%% (Wartość ADC: %d)\n", moisture_level, adc1_get_raw(ADC_CHANNEL));
// //         vTaskDelay(TICKS(5000));
// //     }*/

// }

// Initialize BLE and GATT Server

// void app_main(){
//     nvs_flash_init();
//     init_ble();
//     xTaskCreate(read_dht11_task, "read_dht11_task", 2048, NULL, 5, NULL);

// }

void app_main() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Jeżeli NVS jest uszkodzona, czyścimy ją
        ret = nvs_flash_init();  // Ponowna inicjalizacja
    }
    ESP_ERROR_CHECK(ret);

    // Create task to simulate DHT11 sensor data
    printf("DUPA BLADA\n");
    xTaskCreate(read_dht11_task, "read_dht11_task", 2048, NULL,configMAX_PRIORITIES - 1 ,NULL);

    init_ble();
    //vTaskDelay(pdMS_TO_TICKS(500));

}
