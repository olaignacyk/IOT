#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h" 

#define TICKS(ms) ((ms) / portTICK_PERIOD_MS)  

void readBH1750() {
    i2c_config_t i2c_mst_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000  // szybkość I2C
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_mst_config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
    uint8_t data_wr[1] = {0x10};  //  do uruchomienia pomiaru (tryb ciągły)
    
    while (1) {
        esp_err_t ret = i2c_master_write_to_device(I2C_NUM_0, 0x23, data_wr, sizeof(data_wr), TICKS(1000));
        if (ret != ESP_OK) {
            ESP_LOGE("I2C", "Error sending data: %s", esp_err_to_name(ret));
            vTaskDelay(TICKS(1000));
            continue;
        }
        uint8_t data_rd[2] = {0, 0};  // bufor na odczyt danych
        vTaskDelay(TICKS(180));  
        // odczyt danych z BH1750
        ret = i2c_master_read_from_device(I2C_NUM_0, 0x23, data_rd, sizeof(data_rd), TICKS(1000));
        if (ret != ESP_OK) {
            ESP_LOGE("I2C", "Error reading data: %s", esp_err_to_name(ret));
            vTaskDelay(TICKS(1000));
            continue;
        }
        // konwersja danych na wartość lux
        float lux = (data_rd[0] << 8 | data_rd[1]) / 1.2;
        printf("Raw data: %x %x\n", data_rd[0], data_rd[1]);
        printf("Lux: %f\n", lux);
        vTaskDelay(TICKS(1000));
    }
}

