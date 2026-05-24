#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <hanes_driver.h>

#define SLEEP_TIME_MS CONFIG_APP_HEARTBEAT_PERIOD_MS

#define SENSOR_NODE DT_ALIAS(hanes_led_sensor)

static const struct device* led_sensor = DEVICE_DT_GET(SENSOR_NODE);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    struct sensor_value state;
    const char *msg;

    if (!device_is_ready(led_sensor))
    {
        LOG_ERR("LD2 sensor device not ready");
        return 0;
    }

    LOG_INF("Board name: %s", CONFIG_BOARD);

    while (1)
    {
        int err = sensor_sample_fetch(led_sensor);

        if (err < 0)
        {
            LOG_ERR("sensor_sample_fetch failed (%d)", err);
            k_msleep(SLEEP_TIME_MS);
            continue;
        }

        err = sensor_channel_get(led_sensor, SENSOR_CHAN_PROX, &state);

        if (err < 0)
        {
            LOG_ERR("sensor_channel_get failed (%d)", err);
        }
        else
        {
            hanes_driver_set_message(led_sensor,
                                     state.val1 ? "LD2 -> ON" : "LD2 -> OFF");
            hanes_driver_get_message(led_sensor, &msg);
            LOG_INF("%s, state=%d", msg, state.val1);
        }

        k_msleep(SLEEP_TIME_MS);
    }
}
