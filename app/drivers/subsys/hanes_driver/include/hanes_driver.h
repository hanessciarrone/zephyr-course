#pragma once

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/toolchain.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HANES_DRIVER_MESSAGE_MAX_LEN 64

__subsystem struct hanes_ext_driver_api {
	struct sensor_driver_api sensor_api;
	int (*set_message)(const struct device *dev, const char *msg);
	int (*get_message)(const struct device *dev, const char **msg);
};

static inline int hanes_driver_set_message(const struct device *dev, const char *msg)
{
	return DEVICE_API_GET(hanes_ext, dev)->set_message(dev, msg);
}

static inline int hanes_driver_get_message(const struct device *dev, const char **msg)
{
	return DEVICE_API_GET(hanes_ext, dev)->get_message(dev, msg);
}

#ifdef __cplusplus
}
#endif
