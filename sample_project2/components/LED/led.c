#include "led.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char * TAG = "LED";

void led_init(void)
{
	esp_err_t err = ESP_FAIL;

    gpio_config_t gpio_structure = {
		.intr_type    = GPIO_INTR_DISABLE,
		.mode         = GPIO_MODE_INPUT_OUTPUT,
		.pin_bit_mask = 1ull << GPIO_NUM_5,  // GPIO5 是灯 led
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en   = GPIO_PULLUP_ENABLE,
	};

    err = gpio_config(&gpio_structure);

	if(err != ESP_OK)
	{
		ESP_LOGE(TAG, "gpio_config() error!\r\n");
	}

	gpio_set_level(GPIO_NUM_5, 1);
}
