#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

// Konfiguracja SSID i hasła
const char *ssid = "nazwa";    // Nazwa sieci Wi-Fi (SSID)
const char *pass = "12345678";                    // Hasło do sieci Wi-Fi

#define ESP_WIFI_CHANNEL   1                       // Kanał Wi-Fi
#define MAX_STA_CONN       4                        // Maksymalna liczba podłączonych urządzeń

static const char *TAG = "wifi softAP";            // Tag dla logów

// Funkcja obsługująca zdarzenia Wi-Fi
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station " MACSTR " joined, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station " MACSTR " left, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}

// Funkcja inicjalizująca tryb Access Point
void wifi_init_softap(void) {
    // Inicjalizacja interfejsu sieciowego
    ESP_ERROR_CHECK(esp_netif_init());
    // Tworzenie domyślnej pętli zdarzeń
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // Tworzenie domyślnego interfejsu AP (Access Point)
    esp_netif_create_default_wifi_ap();

    // Inicjalizacja Wi-Fi z domyślnymi ustawieniami
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Rejestracja handlera do obsługi zdarzeń Wi-Fi
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    // Konfiguracja Access Point (AP)
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "",
            .ssid_len = 0,
            .channel = ESP_WIFI_CHANNEL,
            .password = "",
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,  // Tryb uwierzytelniania: WPA2
            .pmf_cfg = {
                .required = true,  // Zarządzanie zabezpieczeniami klucza (PMF)
            },
        },
    };

    // Skopiuj SSID i hasło do struktury
    snprintf((char *)wifi_config.ap.ssid, sizeof(wifi_config.ap.ssid), "%s", ssid);
    wifi_config.ap.ssid_len = strlen((const char *)wifi_config.ap.ssid);

    snprintf((char *)wifi_config.ap.password, sizeof(wifi_config.ap.password), "%s", pass);

    // Jeśli hasło jest puste, ustaw tryb otwarty (bez zabezpieczeń)
    if (strlen(pass) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    // Ustawienie trybu AP (Access Point)
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    // Ustawienie konfiguracji Wi-Fi dla Access Point
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    // Uruchomienie Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ssid, pass, ESP_WIFI_CHANNEL);
}

// Główna funkcja aplikacji
void app_main(void) {
    // Inicjalizacja pamięci nieulotnej NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    // Inicjalizacja trybu Access Point
    wifi_init_softap();
}