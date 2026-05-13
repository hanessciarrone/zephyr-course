#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static int scratch_board_late_init(void)
{
    printk("\nBoard initialized\n");
    k_msleep(50);
    return 0;
}

SYS_INIT(scratch_board_late_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
