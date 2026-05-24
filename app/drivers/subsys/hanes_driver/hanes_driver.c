#define DT_DRV_COMPAT hanes_driver

#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <hanes_driver.h>

LOG_MODULE_REGISTER(hanes_driver, LOG_LEVEL_INF);

struct hanes_driver_config
{
	struct gpio_dt_spec led;
};

struct hanes_driver_data
{
	bool state;
	char message[HANES_DRIVER_MESSAGE_MAX_LEN];
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

static int hanes_driver_set_message_impl(const struct device *dev, const char *msg)
{
	struct hanes_driver_data *data = dev->data;

	if (msg == NULL)
	{
		return -EINVAL;
	}

	strncpy(data->message, msg, HANES_DRIVER_MESSAGE_MAX_LEN - 1);
	data->message[HANES_DRIVER_MESSAGE_MAX_LEN - 1] = '\0';

	return 0;
}

static int hanes_driver_get_message_impl(const struct device *dev, const char **msg)
{
	struct hanes_driver_data *data = dev->data;

	if (msg == NULL)
	{
		return -EINVAL;
	}

	*msg = data->message;
	return 0;
}

static DEVICE_API(hanes_ext, hanes_ext_apis) =
{
	.sensor_api = {
		.sample_fetch = hanes_driver_sample_fetch,
		.channel_get = hanes_driver_channel_get,
	},
	.set_message = hanes_driver_set_message_impl,
	.get_message = hanes_driver_get_message_impl,
};

static int hanes_driver_init(const struct device *dev)
{
	const struct hanes_driver_config *config = dev->config;
	struct hanes_driver_data *data = dev->data;

	if (!device_is_ready(config->led.port))
	{
		return -ENODEV;
	}

	strncpy(data->message, "LD2 ready", HANES_DRIVER_MESSAGE_MAX_LEN - 1);
	data->message[HANES_DRIVER_MESSAGE_MAX_LEN - 1] = '\0';

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
			POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &hanes_ext_apis);

DT_INST_FOREACH_STATUS_OKAY(HANES_DRIVER_INIT);
