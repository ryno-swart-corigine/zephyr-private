/*
 * Copyright (c) 2022 Jerry Chai <chaizhiyong@eswin.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <version.h>
#include <stdlib.h>
#include <string.h>

#include <soc.h>

#define DT_DRV_COMPAT eswin_soc_version
static uint32_t soc_version_base = DT_INST_REG_ADDR(0);

static int eswin_e330_soc_init(void)
{
	uint32_t key;

	key = irq_lock();

#ifndef CONFIG_ESWIN_CLIC
	/* CLIC Direct Mode: All exceptions set pc to BASE. */
	__asm__ volatile ("csrs	mtvec, 0x2\n");

	//enable clic timer
	*(int *)(0x2800404) = 0x01000000;

	//enable clic uart and dma interrupt
	*(int *)(0x2800410) = 0x01010101;
#endif

	irq_unlock(key);

	return 0;
}

SYS_INIT(eswin_e330_soc_init, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

struct VERSION soc_version[SOC_VERSION_TYPES] =
{

	{"FPGA HW:\t\t\t", CHAR_CLASS, SOC_FPGA_HW_SIZE},
	{"CPU type:\t\t\t", CHAR_CLASS, SOC_CPU_TYPE_SIZE},
	{"CPU branch and version:\t\t", INT_CLASS, SOC_CPU_BRANCH_VERSION_SIZE},
	{"CPU reserved:\t\t\t", CHAR_CLASS, SOC_CPU_RESERVED_SIZE},
	{"SOC platform:\t\t\t", CHAR_CLASS, SOC_PLATFORM_SIZE},
	{"SOC branch and version:\t\t", INT_CLASS, SOC_BRANCH_VERSION_SIZE},
	{"FPGA test branch verison:\t", INT_CLASS, FPGA_TEST_BRANCH_VERSION_SIZE}
};

static int eswin_e330_show_version(void)
{
	char version[VERSION_STRING_LEN];
	char *p = version;
	int i;
	int j;

	memcpy(version, (uint32_t *)soc_version_base, 16);

	for(i=0; i < SOC_VERSION_TYPES; i++)
	{
		printk("%s", soc_version[i].type_string);
		for(j=0; j < soc_version[i].type_size;  j++)
		{
			if(soc_version[i].type_class == CHAR_CLASS)
			{
				printk("%c", *p);
			}
			else
			{
				printk("%02x", *p);
			}
			p++;
		}
		printk("\n");
	}

	return 0;
}

SYS_INIT(eswin_e330_show_version, POST_KERNEL, SOC_VERSION_INIT_PRIO);
