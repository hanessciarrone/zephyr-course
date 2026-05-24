#include "zephyr/shell/shell_fprintf.h"
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <hanes_driver.h>

#define SLEEP_TIME_MS CONFIG_APP_HEARTBEAT_PERIOD_MS

#define SENSOR_NODE DT_ALIAS(hanes_led_sensor)

static const struct device* led_sensor = DEVICE_DT_GET(SENSOR_NODE);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* Fordware declaration */
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv);
static int cmd_sensor_get(const struct shell *sh, size_t argc, char **argv);
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv);
static int cmd_sensor_set_data(const struct shell *sh, size_t argc, char **argv);
static int cmd_sensor_get_data(const struct shell *sh, size_t argc, char **argv);

int main(void)
{
    struct sensor_value state;
    const char *msg;

    LOG_INF("Board name: %s", CONFIG_BOARD);

    while (1)
    {
        k_msleep(SLEEP_TIME_MS);
    }
}


static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    const int err = sensor_sample_fetch(led_sensor);

    if (err < 0)
    {
        shell_fprintf(sh, SHELL_VT100_COLOR_RED, "cmd_sensor_fetch failed (%d)\n", err);
    }

    return err;
}

static int cmd_sensor_get(const struct shell *sh, size_t argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    struct sensor_value state;
    const int err = sensor_channel_get(led_sensor, SENSOR_CHAN_PROX, &state);

    if (err < 0)
    {
        shell_fprintf(sh, SHELL_VT100_COLOR_RED, "sensor_channel_get failed (%d)\n", err);
    }
    else
    {
        shell_fprintf(sh, SHELL_VT100_COLOR_DEFAULT, "LED state = %d\n", state.val1);
    }

    return err;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    shell_fprintf(sh, SHELL_VT100_COLOR_GREEN, "Device name: %s and state is %d\n", DEVICE_DT_NAME(SENSOR_NODE), device_is_ready(led_sensor));

    return 0;
}

static int cmd_sensor_set_data(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 1)
    {
        shell_error(sh, "Argument missing");
    }

    const int length = strlen(argv[1]);

    if (length >= CONFIG_HANES_DRIVER_MESSAGE_MAX_LEN)
    {
        shell_error(sh, "Message too long");
    }

    hanes_driver_set_message(led_sensor, argv[1]);

    return 0;
}

static int cmd_sensor_get_data(const struct shell *sh, size_t argc, char **argv)
{

    const char *msg;

    hanes_driver_get_message(led_sensor, &msg);
    shell_fprintf(sh, SHELL_VT100_COLOR_DEFAULT, "Device data = %s\n", msg);

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_sensor,
    SHELL_CMD(fetch, NULL, "Turn ON/OFF sensor led",    cmd_sensor_fetch),
    SHELL_CMD(read,  NULL, "Get sensor led state",      cmd_sensor_get),
    SHELL_CMD(info,  NULL, "Get information of device", cmd_sensor_info),
    SHELL_CMD(get,   NULL, "Get the message device",    cmd_sensor_get_data),
    SHELL_CMD_ARG(set, NULL, "Set the message device", cmd_sensor_set_data, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "Sensor commands", NULL);
