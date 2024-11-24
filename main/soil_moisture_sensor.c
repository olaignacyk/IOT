#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_system.h"

#define ADC_CHANNEL ADC1_CHANNEL_7     
#define ADC_WIDTH ADC_WIDTH_BIT_12      
#define ADC_ATTENUATION ADC_ATTEN_DB_11 

void soil_moisture_sensor_init(void) {
    adc1_config_width(ADC_WIDTH); 
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTENUATION);
    printf("Czujnik wilgotności gleby zainicjowany na kanale %d\n", ADC_CHANNEL);
}

float soil_moisture_sensor_read(void) {
    int adc_value = 0;
    adc_value = adc1_get_raw(ADC_CHANNEL); 
    float moisture_level = 100 - ((float)adc_value * 100 / 4095); 
    return moisture_level;
}

