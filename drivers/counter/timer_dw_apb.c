/*
 * Copyright (c) 2022 Jerry Chai <chaizhiyong@eswin.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT snps_dw_apb_timer

#include <zephyr/drivers/counter.h>
#include <zephyr/device.h>
#include <errno.h>
#include <zephyr/init.h>
#include <soc.h>
#include <zephyr/drivers/clock_control/arm_clock_control.h>
#include <zephyr/irq.h>


typedef void (*timer_config_func_t)(const struct device *dev);

struct timer_dw_apb {
	/* Offset: 0x000 (R/W) reload value register */
	volatile uint32_t reload;
	/* Offset: 0x004 (R/W) current value register */
	volatile uint32_t value;
	/* Offset: 0x008 (R/W) control register */
	volatile uint32_t ctrl;
	/* Offset: 0x00C ( /W) interruptclear register */
	volatile uint32_t intclear;
	/* Offset: 0x010 (R/ ) interrupt status register */
	volatile uint32_t intstatus;
};

#define TIMER_CTRL_IRQ_DISABLE  (1 << 2)
#define TIMER_CTRL_MODE         (1 << 1)
#define TIMER_CTRL_EN           (1 << 0)
#define TIMER_CTRL_INT_CLEAR    (1 << 0)

struct tmr_dw_apb_cfg {
	struct counter_config_info info;
	volatile struct timer_dw_apb *timer;
	timer_config_func_t timer_config_func;
};

struct tmr_dw_apb_dev_data {
	counter_top_callback_t top_callback;
	void *top_user_data;

	uint32_t load;
};

static int tmr_dw_apb_start(const struct device *dev)
{
	const struct tmr_dw_apb_cfg * const cfg =	dev->config;
	struct tmr_dw_apb_dev_data *data = dev->data;

	/* Set the timer reload to count */
	cfg->timer->reload = data->load;

	cfg->timer->ctrl |= TIMER_CTRL_EN;

	return 0;
}

static int tmr_dw_apb_stop(const struct device *dev)
{
	const struct tmr_dw_apb_cfg * const cfg = dev->config;
	/* Disable the timer */
	cfg->timer->ctrl &= ~TIMER_CTRL_EN;

	return 0;
}

static int tmr_dw_apb_get_value(const struct device *dev, uint32_t *ticks)
{
	const struct tmr_dw_apb_cfg * const cfg = dev->config;
	struct tmr_dw_apb_dev_data *data = dev->data;

	/* Get Counter Value */
	*ticks = data->load - cfg->timer->value;
	return 0;
}

static int tmr_dw_apb_set_top_value(const struct device *dev,
				       const struct counter_top_cfg *top_cfg)
{
	const struct tmr_dw_apb_cfg * const cfg = dev->config;
	struct tmr_dw_apb_dev_data *data = dev->data;

	/* Counter is always reset when top value is updated. */
	if (top_cfg->flags & COUNTER_TOP_CFG_DONT_RESET) {
		return -ENOTSUP;
	}

	data->top_callback = top_cfg->callback;
	data->top_user_data = top_cfg->user_data;

	/* Store the reload value */
	data->load = top_cfg->ticks;

	/* Set value register to count */
	cfg->timer->value = top_cfg->ticks;

	/* Set the timer reload to count */
	cfg->timer->reload = top_cfg->ticks;

	/* Enable IRQ */
	cfg->timer->ctrl |= TIMER_CTRL_MODE;

	return 0;
}

static uint32_t tmr_dw_apb_get_top_value(const struct device *dev)
{
	struct tmr_dw_apb_dev_data *data = dev->data;

	uint32_t ticks = data->load;

	return ticks;
}

static uint32_t tmr_dw_apb_get_pending_int(const struct device *dev)
{
	const struct tmr_dw_apb_cfg * const cfg = dev->config;

	return cfg->timer->intstatus;
}

static const struct counter_driver_api tmr_dw_apb_api = {
	.start = tmr_dw_apb_start,
	.stop = tmr_dw_apb_stop,
	.get_value = tmr_dw_apb_get_value,
	.set_top_value = tmr_dw_apb_set_top_value,
	.get_pending_int = tmr_dw_apb_get_pending_int,
	.get_top_value = tmr_dw_apb_get_top_value,
};

static void tmr_dw_apb_isr(void *arg)
{
	const struct device *dev = (const struct device *)arg;
	struct tmr_dw_apb_dev_data *data = dev->data;
	const struct tmr_dw_apb_cfg * const cfg = dev->config;
	int clear;

	clear = cfg->timer->intclear;

	if (data->top_callback) {
		data->top_callback(dev, data->top_user_data);
	}
}

static int tmr_dw_apb_init(const struct device *dev)
{
	const struct tmr_dw_apb_cfg * const cfg = dev->config;

	cfg->timer_config_func(dev);

	return 0;
}
//#define DT_N_S_soc_S_dw_timer_400b0078_P_clock_frequency   10000000 
//.freq = DT_INST_PROP(inst, clock_frequency),
#define TIMER_DW_APB_INIT(inst)						\
	static void timer_dw_apb_config_##inst(const struct device *dev); \
									\
	static const struct tmr_dw_apb_cfg tmr_dw_apb_cfg_##inst = { \
		.info = {						\
			.max_top_value = UINT32_MAX,			\
			.freq = 10000000,		\
			.flags = 0,					\
			.channels = 0U,					\
		},							\
		.timer = ((volatile struct timer_dw_apb *)DT_INST_REG_ADDR(inst)), \
		.timer_config_func = timer_dw_apb_config_##inst,	\
	};								\
									\
	static struct tmr_dw_apb_dev_data tmr_dw_apb_dev_data_##inst = { \
		.load = UINT32_MAX,					\
	};								\
									\
	DEVICE_DT_INST_DEFINE(inst,					\
			    tmr_dw_apb_init,				\
			    NULL,			\
			    &tmr_dw_apb_dev_data_##inst,		\
			    &tmr_dw_apb_cfg_##inst, POST_KERNEL,	\
			    CONFIG_COUNTER_INIT_PRIORITY,		\
			    &tmr_dw_apb_api);			\
									\
	static void timer_dw_apb_config_##inst(const struct device *dev) \
	{								\
		IRQ_CONNECT(DT_INST_IRQN(inst),				\
				0,                            \
			    tmr_dw_apb_isr,				\
			    DEVICE_DT_INST_GET(inst),			\
			    0);						\
		irq_enable(DT_INST_IRQN(inst));				\
	}

DT_INST_FOREACH_STATUS_OKAY(TIMER_DW_APB_INIT)
