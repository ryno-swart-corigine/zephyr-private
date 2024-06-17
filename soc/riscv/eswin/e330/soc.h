/*
 * Copyright (c) 2022 Jerry Chai <chaizhiyong@eswin.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file SoC configuration macros for the Eswin E315S processor
 */

#ifndef __RISCV_ESWIN_E315S_SOC_H_
#define __RISCV_ESWIN_E315S_SOC_H_

#include <soc_common.h>
#include "zephyr/devicetree.h"

/* Timer configuration */
#define RISCV_MTIME_BASE             0x0200BFF8
#define RISCV_MTIMECMP_BASE          0x02004000

/* lib-c hooks required RAM defined variables */
#define RISCV_RAM_BASE               CONFIG_SRAM_BASE_ADDRESS
#define RISCV_RAM_SIZE               KB(CONFIG_SRAM_SIZE)

#define SOC_FPGA_HW_SIZE                 3
#define SOC_CPU_TYPE_SIZE                5
#define SOC_CPU_BRANCH_VERSION_SIZE      2
#define SOC_CPU_RESERVED_SIZE            1
#define SOC_PLATFORM_SIZE                1
#define SOC_BRANCH_VERSION_SIZE          2
#define FPGA_TEST_BRANCH_VERSION_SIZE    2

#define SOC_VERSION_TYPES				 7
#define VERSION_STRING_LEN				 16

#define SOC_VERSION_INIT_PRIO            32

#define CLIC_SOFTWARE_INTERRUPT_INDEX    12

#ifndef _ASMLANGUAGE
enum TYPE_CLASS{

	CHAR_CLASS,
	INT_CLASS
};

struct VERSION
{
	char* type_string;
	enum TYPE_CLASS type_class;
	int  type_size;
};
#endif /* !_ASMLANGUAGE */

#endif /* __RISCV_ESWIN_E315S_SOC_H_ */
