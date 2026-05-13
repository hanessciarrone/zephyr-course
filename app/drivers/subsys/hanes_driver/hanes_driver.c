#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(hanes_driver, LOG_LEVEL_INF);

struct hanes_driver_config
{
	struct gpio_dt_spec led;
};

struct hanes_driver_data
{
	bool state;
};

static int hanes_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	const struct hanes_driver_config *config = dev->config;
	struct hanes_driver_data *data = dev->data;

	int ret = gpio_pin_toggle_dt(&config->led);

	if (ret < 0)
	{
		return ret;
	}

	data->state = gpio_pin_get_dt(&config->led);
	return 0;
}

static int hanes_driver_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val)
{
	struct hanes_driver_data *data = dev->data;

	val->val1 = data->state ? 1 : 0;
	val->val2 = 0;
	return 0;
}

static const struct sensor_driver_api hanes_driver_api =
{
	.sample_fetch = hanes_driver_sample_fetch,
	.channel_get = hanes_driver_channel_get,
};

static int hanes_driver_init(const struct device *dev)
{
	const struct hanes_driver_config *config = dev->config;

	if (!device_is_ready(config->led.port))
	{
		return -ENODEV;
	}

	return gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_ACTIVE);
}

#define HANES_DRIVER_INIT(inst)                                             \
	static struct hanes_driver_data hanes_driver_data_##inst;               \
	static const struct hanes_driver_config hanes_driver_config_##inst = {  \
		.led = GPIO_DT_SPEC_INST_GET(inst, gpios),                          \
	};                                                                      \
	SENSOR_DEVICE_DT_INST_DEFINE(inst,                                      \
			&hanes_driver_init, NULL,                                       \
			&hanes_driver_data_##inst, &hanes_driver_config_##inst,         \
			POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &hanes_driver_api);


#define DT_DRV_COMPAT hanes_driver

DT_INST_FOREACH_STATUS_OKAY(HANES_DRIVER_INIT);
