#include "esp_timer.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht11.h"

static gpio_num_t dht_gpio;
static int64_t last_read_time = -2000000;
static struct dht11_reading last_read;


static void delay_us(uint32_t us)
{
    int64_t start_time = esp_timer_get_time();
    while ((esp_timer_get_time() - start_time) < us)
    {
        // Busy-wait loop for accurate delay
    }
}


static int _waitOrTimeout(uint16_t microSeconds, int level)
{

    int micros_ticks = 0;
    while (gpio_get_level(dht_gpio) == level)
    {
        if (micros_ticks++ > microSeconds)
            return DHT11_TIMEOUT_ERROR;
        // ets_delay_us(1);
            delay_us(1);
    }
    return micros_ticks;
}

static int _checkCRC(uint8_t data[])
{
    if (data[4] == (data[0] + data[1] + data[2] + data[3]))
        return DHT11_OK;
    else
        return DHT11_CRC_ERROR;
}

static void _sendStartSignal()
{
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    delay_us(20 * 1000);
    gpio_set_level(dht_gpio, 1);
    delay_us(40);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

static int _checkResponse()
{
    if (_waitOrTimeout(120, 0) == DHT11_TIMEOUT_ERROR)
    {
        return DHT11_TIMEOUT_ERROR;
    }
    if (_waitOrTimeout(120, 1) == DHT11_TIMEOUT_ERROR)
    {
        return DHT11_TIMEOUT_ERROR;
    }

    return DHT11_OK;
}

static struct dht11_reading _timeoutError()
{
    struct dht11_reading timeoutError = {DHT11_TIMEOUT_ERROR, -1, -1};
    return timeoutError;
}

static struct dht11_reading _crcError()
{
    struct dht11_reading crcError = {DHT11_CRC_ERROR, -1, -1};
    return crcError;
}

void DHT11_init(gpio_num_t gpio_num)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    dht_gpio = gpio_num;
}

struct dht11_reading DHT11_read()
{
    if (esp_timer_get_time() - 2000000 < last_read_time)
    {
        return last_read;
    }

    last_read_time = esp_timer_get_time();
    uint8_t data[5] = {0, 0, 0, 0, 0};

    _sendStartSignal();

    if (_checkResponse() == DHT11_TIMEOUT_ERROR)
    {
        printf("timeout 1\n");
        return last_read = _timeoutError();
    }
    for (int i = 0; i < 40; i++)
    {
        if (_waitOrTimeout(50, 0) == DHT11_TIMEOUT_ERROR)
        {
            printf("timeout 2\n");
            return last_read = _timeoutError();
        }

        if (_waitOrTimeout(70, 1) > 28)
        {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    if (_checkCRC(data) != DHT11_CRC_ERROR)
    {
        last_read.status = DHT11_OK;
        last_read.temperature = data[2];
        last_read.humidity = data[0];
        return last_read;
    }
    else
    {
        return last_read = _crcError();
    }
}
