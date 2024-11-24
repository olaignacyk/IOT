// // #include <stdio.h>
// // #include <string.h>
// // #include "esp_log.h"
// // #include "esp_bt.h"
// // #include "esp_bt_main.h"
// // #include "esp_gatts_api.h"
// // #include "esp_gap_ble_api.h"
// // #include "esp_gatt_common_api.h"
// // #include "freertos/FreeRTOS.h"
// // #include "freertos/task.h"

// // #include "dht11.h"

// // static const char *TAG_GATT = "BLE_GATT_SERVER";

// // #define GATTS_SERVICE_UUID  0x00FF
// // #define GATTS_CHAR_UUID     0xFF01
// // #define GATTS_NUM_HANDLE    4

// // #define DEVICE_NAME "ESP32_DHT11"
// // #define CHAR_VAL_LEN_MAX 0x40

// // static uint16_t service_handle;
// // static esp_gatt_char_prop_t char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

// // static esp_attr_value_t gatts_char_value = {
// //     .attr_max_len = CHAR_VAL_LEN_MAX,
// //     .attr_len = sizeof(uint8_t),
// //     .attr_value = NULL,
// // };

// // static esp_ble_adv_data_t adv_data = {
// //     .set_scan_rsp = false,
// //     .include_name = true,
// //     .include_txpower = true,
// //     .min_interval = 0x20,
// //     .max_interval = 0x40,
// //     .appearance = 0x00,
// //     .manufacturer_len = 0,
// //     .p_manufacturer_data = NULL,
// //     .service_data_len = 0,
// //     .p_service_data = NULL,
// //     .service_uuid_len = 0,
// //     .p_service_uuid = NULL,
// //     .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
// // };

// // static esp_ble_adv_params_t adv_params = {
// //     .adv_int_min = 0x20,
// //     .adv_int_max = 0x40,
// //     .adv_type = ADV_TYPE_IND,
// //     .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
// //     .peer_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
// //     .peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
// //     .channel_map = ADV_CHNL_ALL,
// //     .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
// // };

// // static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
// //     switch (event) {
// //         case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
// //             esp_ble_gap_start_advertising(&adv_params);
// //             break;
// //         default:
// //             ESP_LOGI(TAG_GATT, "Unhandled GAP event: %d", event);
// //             break;
// //     }
// // }
// // static uint8_t client_char_config[] = {0x00, 0x00};  // Notifications disabled by default

// // esp_gatts_attr_db_t gatt_db[GATTS_NUM_HANDLE] = {
// //     // Service Declaration
// //     [0] = { 
// //         {ESP_GATT_AUTO_HANDLE, ESP_GATT_PERM_READ, ESP_GATT_TYPE_PRIMARY_SERVICE, sizeof(uint16_t), (uint8_t*)&service_uuid}, 
// //     },
// //     // Characteristic Declaration
// //     [1] = { 
// //         {ESP_GATT_AUTO_HANDLE, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, ESP_GATT_TYPE_CHAR_DECLARATION, sizeof(uint8_t), (uint8_t*)&char_prop}, 
// //     },
// //     // Characteristic Value
// //     [2] = {
// //         {ESP_GATT_AUTO_HANDLE, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, ESP_GATT_TYPE_CHAR_VALUE, sizeof(uint8_t), (uint8_t*)&char_val},
// //     },
// //     // Client Characteristic Configuration Descriptor (for notifications)
// //     [3] = {
// //         {ESP_GATT_AUTO_HANDLE, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, ESP_GATT_TYPE_CHAR_DESCRIPTOR, sizeof(client_char_config), client_char_config},
// //     }
// // };


// // static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
// //     switch (event) {
// //         case ESP_GATTS_REG_EVT:
// //             ESP_LOGI(TAG_GATT, "ESP_GATTS_REG_EVT");
// //             esp_ble_gap_set_device_name(DEVICE_NAME);
// //             esp_ble_gap_config_adv_data(&adv_data);

// //             esp_gatt_srvc_id_t service_id = {
// //                 .is_primary = true,
// //                 .id = {
// //                     .inst_id = 0,
// //                     .uuid = {
// //                         .len = ESP_UUID_LEN_16,
// //                         .uuid = {.uuid16 = GATTS_SERVICE_UUID},
// //                     },
// //                 },
// //             };

// //             esp_ble_gatts_create_service(gatts_if, &service_id, GATTS_NUM_HANDLE);
// //             break;

// //         case ESP_GATTS_CREATE_EVT:
// //             ESP_LOGI(TAG_GATT, "SERVICE_CREATED_EVT, handle: %d", param->create.service_handle);
// //             service_handle = param->create.service_handle;

// //             esp_ble_gatts_start_service(service_handle);
// //             esp_ble_gatts_add_char(service_handle,
// //                                    &(esp_bt_uuid_t){.len = ESP_UUID_LEN_16, .uuid.uuid16 = GATTS_CHAR_UUID},
// //                                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
// //                                    char_property,
// //                                    &gatts_char_value, NULL);
// //             break;

// //         case ESP_GATTS_ADD_CHAR_EVT:
// //             ESP_LOGI(TAG_GATT, "ADD_CHAR_EVT");
// //             break;

// //         case ESP_GATTS_READ_EVT:
// //             ESP_LOGI(TAG_GATT, "READ_EVT");
// //             break;

// //         case ESP_GATTS_WRITE_EVT:
// //             ESP_LOGI(TAG_GATT, "WRITE_EVT");
// //             break;

// //         case ESP_GATTS_CONNECT_EVT:
// //             ESP_LOGI(TAG_GATT, "CONNECT_EVT");
// //             break;

// //         case ESP_GATTS_DISCONNECT_EVT:
// //             ESP_LOGI(TAG_GATT, "DISCONNECT_EVT");
// //             esp_ble_gap_start_advertising(&adv_params);
// //             break;

// //         default:
// //             ESP_LOGI(TAG_GATT, "Unhandled GATT event: %d", event);
// //             break;
// //     }
// // }

// // void init_ble() {
// //     esp_err_t ret;

// //     ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
// //     if (ret) {
// //         ESP_LOGE(TAG_GATT, "Bluetooth controller memory release failed: %s", esp_err_to_name(ret));
// //         return;
// //     }

// //     esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
// //     ret = esp_bt_controller_init(&bt_cfg);
// //     if (ret) {
// //         ESP_LOGE(TAG_GATT, "Bluetooth controller initialization failed: %s", esp_err_to_name(ret));
// //         return;
// //     }

// //     ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
// //     if (ret) {
// //         ESP_LOGE(TAG_GATT, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
// //         return;
// //     }

// //     ret = esp_bluedroid_init();
// //     if (ret) {
// //         ESP_LOGE(TAG_GATT, "Bluedroid initialization failed: %s", esp_err_to_name(ret));
// //         return;
// //     }

// //     ret = esp_bluedroid_enable();
// //     if (ret) {
// //         ESP_LOGE(TAG_GATT, "Bluedroid enable failed: %s", esp_err_to_name(ret));
// //         return;
// //     }

// //     ret = esp_ble_gap_register_callback(gap_event_handler);
// //     if (ret) {
// //         ESP_LOGE(TAG_GATT, "GAP callback register failed: %s", esp_err_to_name(ret));
// //         return;
// //     }

// //     ret = esp_ble_gatts_register_callback(gatts_event_handler);
// //     if (ret) {
// //         ESP_LOGE(TAG_GATT, "GATTS callback register failed: %s", esp_err_to_name(ret));
// //         return;
// //     }

// //     ret = esp_ble_gatts_app_register(0);
// //     if (ret) {
// //         ESP_LOGE(TAG_GATT, "GATTS app register failed: %s", esp_err_to_name(ret));
// //         return;
// //     }

// //     ESP_LOGI(TAG_GATT, "BLE initialized successfully");
// // // }
// #include <stdio.h>
// #include <string.h>
// #include "esp_log.h"
// #include "esp_bt.h"
// #include "esp_bt_main.h"
// #include "esp_gatts_api.h"
// #include "esp_gap_ble_api.h"
// #include "esp_gatt_common_api.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #include "dht11.h"

// // BLE definitions
// static const char *TAG_GATT = "BLE_GATT_SERVER";

// // UUIDs for service and characteristic
// #define GATTS_SERVICE_UUID  0x00FF
// #define GATTS_CHAR_UUID     0xFF01
// #define GATTS_NUM_HANDLE    4

// #define DEVICE_NAME "ESP32_DHT11"
// #define CHAR_VAL_LEN_MAX 0x40


// // DHT11 pin configuration
// #define DHT11_PIN 4

// // Variables for BLE
// static uint16_t service_handle;
// static esp_gatt_char_prop_t char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

// // Descriptor for characteristic (used for notifications)
// static uint8_t client_char_config[] = {0x00, 0x00};  // Notifications disabled by default

// // DHT11 data
// static uint8_t sensor_data[CHAR_VAL_LEN_MAX];




// // GATT characteristic value
// static esp_attr_value_t gatts_char_value = {
//     .attr_max_len = CHAR_VAL_LEN_MAX,
//     .attr_len = sizeof(sensor_data),
//     .attr_value = sensor_data,  // Will hold the sensor data (temperature, humidity)
// };

// // BLE advertising data
// static esp_ble_adv_data_t adv_data = {
//     .set_scan_rsp = false,
//     .include_name = true,
//     .include_txpower = true,
//     .min_interval = 0x20,
//     .max_interval = 0x40,
//     .appearance = 0x00,
//     .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
// };

// // BLE advertising parameters
// static esp_ble_adv_params_t adv_params = {
//     .adv_int_min = 0x20,
//     .adv_int_max = 0x40,
//     .adv_type = ADV_TYPE_IND,
//     .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
//     .peer_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//     .peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
//     .channel_map = ADV_CHNL_ALL,
//     .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
// };

// // GATT Server Event Handler
// static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
//     switch (event) {
//         case ESP_GATTS_REG_EVT:
//             ESP_LOGI(TAG_GATT, "ESP_GATTS_REG_EVT");
//             esp_ble_gap_set_device_name(DEVICE_NAME);
//             esp_ble_gap_config_adv_data(&adv_data);
//             esp_gatt_srvc_id_t service_id = {
//                 .is_primary = true,
//                 .id = {
//                     .inst_id = 0,
//                     .uuid = {
//                         .len = ESP_UUID_LEN_16,
//                         .uuid = {.uuid16 = GATTS_SERVICE_UUID},
//                     },
//                 },
//             };
//             esp_ble_gatts_create_service(gatts_if, &service_id, GATTS_NUM_HANDLE);
//             break;

//         case ESP_GATTS_CREATE_EVT:
//             ESP_LOGI(TAG_GATT, "SERVICE_CREATED_EVT, handle: %d", param->create.service_handle);
//             service_handle = param->create.service_handle;
//             esp_ble_gatts_start_service(service_handle);
//             esp_ble_gatts_add_char(service_handle,
//                                    &(esp_bt_uuid_t){.len = ESP_UUID_LEN_16, .uuid.uuid16 = GATTS_CHAR_UUID},
//                                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
//                                    char_property,
//                                    &gatts_char_value, NULL);
//             break;

//         case ESP_GATTS_ADD_CHAR_EVT:
//             ESP_LOGI(TAG_GATT, "ADD_CHAR_EVT");
//             break;

//         case ESP_GATTS_READ_EVT:
//             ESP_LOGI(TAG_GATT, "READ_EVT");
//             break;

//         case ESP_GATTS_WRITE_EVT:
//             ESP_LOGI(TAG_GATT, "WRITE_EVT");
//             break;

//         case ESP_GATTS_CONNECT_EVT:
//             ESP_LOGI(TAG_GATT, "CONNECT_EVT");
//             break;

//         case ESP_GATTS_DISCONNECT_EVT:
//             ESP_LOGI(TAG_GATT, "DISCONNECT_EVT");
//             esp_ble_gap_start_advertising(&adv_params);
//             break;

//         default:
//             ESP_LOGI(TAG_GATT, "Unhandled GATT event: %d", event);
//             break;
//     }
// }

// // GAP Event Handler (advertising related)
// static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
//     switch (event) {
//         case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
//             esp_ble_gap_start_advertising(&adv_params);
//             break;
//         default:
//             ESP_LOGI(TAG_GATT, "Unhandled GAP event: %d", event);
//             break;
//     }
// }

// // Initialize BLE
// void init_ble() {
//     esp_err_t ret;

//     ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
//     if (ret) {
//         ESP_LOGE(TAG_GATT, "Bluetooth controller memory release failed: %s", esp_err_to_name(ret));
//         return;
//     }

//     esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
//     ret = esp_bt_controller_init(&bt_cfg);
//     if (ret) {
//         ESP_LOGE(TAG_GATT, "Bluetooth controller initialization failed: %s", esp_err_to_name(ret));
//         return;
//     }

//     ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
//     if (ret) {
//         ESP_LOGE(TAG_GATT, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
//         return;
//     }

//     ret = esp_bluedroid_init();
//     if (ret) {
//         ESP_LOGE(TAG_GATT, "Bluedroid initialization failed: %s", esp_err_to_name(ret));
//         return;
//     }

//     ret = esp_bluedroid_enable();
//     if (ret) {
//         ESP_LOGE(TAG_GATT, "Bluedroid enable failed: %s", esp_err_to_name(ret));
//         return;
//     }

//     ret = esp_ble_gap_register_callback(gap_event_handler);
//     if (ret) {
//         ESP_LOGE(TAG_GATT, "GAP callback register failed: %s", esp_err_to_name(ret));
//         return;
//     }

//     ret = esp_ble_gatts_register_callback(gatts_event_handler);
//     if (ret) {
//         ESP_LOGE(TAG_GATT, "GATTS callback register failed: %s", esp_err_to_name(ret));
//         return;
//     }

//     ret = esp_ble_gatts_app_register(0);
//     if (ret) {
//         ESP_LOGE(TAG_GATT, "GATTS app register failed: %s", esp_err_to_name(ret));
//         return;
//     }

//     ESP_LOGI(TAG_GATT, "BLE initialized successfully");
// }
// void read_dht11_task(void *arg)
// {
//     uint8_t test_temperature = 22; // Stała temperatura
//     uint8_t test_humidity = 45;    // Stała wilgotność

//     while (1)
//     {
//         // Wyświetlanie danych w logach
//         ESP_LOGI(TAG_GATT, "Test data: Temperature: %d°C, Humidity: %d%%", test_temperature, test_humidity);

//         // Symulacja opóźnienia odczytu
//         vTaskDelay(pdMS_TO_TICKS(2000)); // Opóźnienie 2 sekundy
//     }
// }

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gatts_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_gap_ble_api.h"
#include "esp_log.h"
#include "dht11.h"

// BLE definitions
static const char *TAG_GATT = "BLE_GATT_SERVER";

// UUIDs for service and characteristic
#define GATTS_SERVICE_UUID  0x00FF
#define GATTS_CHAR_UUID     0xFF01
#define GATTS_NUM_HANDLE    4

#define DEVICE_NAME "ESP32_DHT11"
#define CHAR_VAL_LEN_MAX 0x40


// DHT11 pin configuration
#define DHT11_PIN 4

// Variables for BLE
static uint16_t service_handle;
static esp_gatt_char_prop_t char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

// Descriptor for characteristic (used for notifications)
static uint8_t client_char_config[] = {0x00, 0x00};  // Notifications disabled by default

// DHT11 data
static uint8_t sensor_data[CHAR_VAL_LEN_MAX];

// GATT characteristic value
static esp_attr_value_t gatts_char_value = {
    .attr_max_len = CHAR_VAL_LEN_MAX,
    .attr_len = sizeof(sensor_data),
    .attr_value = sensor_data,  // Will hold the sensor data (temperature, humidity)
};

// BLE advertising data
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// BLE advertising parameters
static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .peer_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// Global variables for GATT interface, connection ID and characteristic handle
static esp_gatt_if_t gatts_if = ESP_GATT_IF_NONE;  // GATT interface
static uint16_t conn_id = 0xFFFF;  // Connection ID (initialized to invalid value)
static uint16_t char_handle = 0;  // Characteristic handle

// GATT Server Event Handler
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
        case ESP_GATTS_REG_EVT:
            ESP_LOGI(TAG_GATT, "ESP_GATTS_REG_EVT");
            gatts_if = gatts_if;  // Save the GATT interface
            esp_ble_gap_set_device_name(DEVICE_NAME);
            esp_ble_gap_config_adv_data(&adv_data);

            esp_ble_gap_start_advertising(&adv_params);
            
            esp_gatt_srvc_id_t service_id = {
                .is_primary = true,
                .id = {
                    .inst_id = 0,
                    .uuid = {
                        .len = ESP_UUID_LEN_16,
                        .uuid = {.uuid16 = GATTS_SERVICE_UUID},
                    },
                },
            };
            esp_ble_gatts_create_service(gatts_if, &service_id, GATTS_NUM_HANDLE);
            break;

        case ESP_GATTS_CREATE_EVT:
            ESP_LOGI(TAG_GATT, "SERVICE_CREATED_EVT, handle: %d", param->create.service_handle);
            service_handle = param->create.service_handle;
            esp_ble_gatts_start_service(service_handle);
            esp_ble_gatts_add_char(service_handle,
                                   &(esp_bt_uuid_t){.len = ESP_UUID_LEN_16, .uuid.uuid16 = GATTS_CHAR_UUID},
                                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                   char_property,
                                   &gatts_char_value, NULL);
            break;

        case ESP_GATTS_ADD_CHAR_EVT:
            ESP_LOGI(TAG_GATT, "ADD_CHAR_EVT");
            char_handle = param->add_char.attr_handle;  // Save the characteristic handle
            break;

    case ESP_GATTS_READ_EVT: {
        ESP_LOGI(TAG_GATT, "Characteristic read, conn_id %d, trans_id %" PRIu32 ", handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        static uint8_t counter = 0;
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 3;
        rsp.attr_value.value[0] = sensor_data[0];
        rsp.attr_value.value[1] = sensor_data[1];
        rsp.attr_value.value[2] = counter++;
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);
        break;
    }

        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(TAG_GATT, "WRITE_EVT");
            break;

        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(TAG_GATT, "CONNECT_EVT");
            conn_id = param->connect.conn_id;  // Save the connection ID
            break;

        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG_GATT, "DISCONNECT_EVT");
            conn_id = 0xFFFF;  // Reset the connection ID
            esp_ble_gap_start_advertising(&adv_params);
            break;

        default:
            ESP_LOGI(TAG_GATT, "Unhandled GATT event: %d", event);
            break;
    }
}
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {


        case ESP_GAP_BLE_ADV_TERMINATED_EVT:  
            ESP_LOGI(TAG_GATT, "Advertising terminated");
            break;

        case ESP_GAP_BLE_SCAN_TIMEOUT_EVT: 
            ESP_LOGI(TAG_GATT, "Scan timeout");
            break;
        default:
            ESP_LOGI(TAG_GATT, "Unhandled GAP event: %d", event);
            break;
    }
}





// Initialize BLE
void init_ble() {
    esp_err_t ret;

    ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret) {
        ESP_LOGE(TAG_GATT, "Bluetooth controller memory release failed: %s", esp_err_to_name(ret));
        return;
    }

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG_GATT, "Bluetooth controller initialization failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG_GATT, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG_GATT, "Bluedroid initialization failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG_GATT, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG_GATT, "GAP callback register failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(TAG_GATT, "GATTS callback register failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_app_register(0);
    if (ret) {
        ESP_LOGE(TAG_GATT, "GATTS app register failed: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG_GATT, "BLE initialized successfully");
}

// void send_fake_dht11_task(void *arg) {
//     while (1) {
//         printf("Start\n");
//         // if (conn_id != 0xFFFF && gatts_if != ESP_GATT_IF_NONE) {  // Ensure GATT interface is available
//             uint8_t temperature = 22;
//             uint8_t humidity = 45;
//             sensor_data[0] = temperature;
//             sensor_data[1] = humidity;

//             // Send notification to the client
//             //esp_ble_gatts_send_indicate(gatts_if, conn_id, char_handle, sizeof(sensor_data), sensor_data, false);
        
//         vTaskDelay(pdMS_TO_TICKS(2000));  // Send data every 2 seconds
//         printf("Koniec\n");
//     }
// }

 void read_dht11_task(void *arg)
{
    DHT11_init(GPIO_NUM_4);

    while (1)
    {
        struct dht11_reading reading = DHT11_read();
        if (reading.status == DHT11_OK)
        {
            
            sensor_data[0] = reading.temperature;
            sensor_data[1] = reading.humidity;
            printf("%d %d\n",reading.temperature, reading.humidity);
            
        }
        else
        {
            printf("Error reading DHT11 sensor. Status code: %d\n", reading.status);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}