/*
 * Copyright (c) 2022 Jerry Chai <chaizhiyong@eswin.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Driver for Eswin's Core Interrupt Controller
 */
#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/sys/util.h>
#include <zephyr/init.h>
#include <soc.h>
#include <zephyr/sw_isr_table.h>

union CLICCFG {
	struct {
		uint8_t nvbits : 1;		/* selective hardware vectoring RW type write 1/0 to enable/disable SHV for all fast interrupts */
		uint8_t nlbits : 3;		/* nlbits - number of level bits RW type to determines the number of the level bits in all cicintcfg* registers */
		uint8_t _reserved1 : 1; /* [6:5] nmbits - number of mode bits R type tied to 0, indicates machine mode interrupt support only. */
		uint8_t nmbits : 2;
		uint8_t _reserved2 : 1;
	} b;
	uint8_t w;
};

union CLICINTIP {
	struct {
		/** Interrupt Pending */
		uint8_t IP : 1;
		uint8_t reserved0 : 7;
	} b;
	uint8_t w;
};

union CLICINTIE {
	struct {
		/** Interrupt Enabled */
		uint8_t IE : 1;
		uint8_t reserved0 : 7;
	} b;
	uint8_t w;
};

/* param: level_and_priority
 * for example, for the 7-bit field[7:1]
 * if ciccfg.nlbits = 0, then level=255,  [7:1]priority
 * if ciccfg.nlbits = 1, then [7:7]level, [6:1]priority
 * if ciccfg.nlbits = 2, then [7:6]level, [5:1]priority
 * if ciccfg.nlbits = 3, then [7:5]level, [4:1]priority
 * if ciccfg.nlbits = 4, then [7:4]level, [3:1]priority
 * if ciccfg.nlbits = 5, then [7:3]level, [2:1]priority
 * if ciccfg.nlbits = 6, then [7:2]level, [1]priority
 * if ciccfg.nlbits = 7, then [7:1]level, no priority
*/
union CLICINTATTR {
	struct {
		/** 0: non-vectored 1:vectored */
		uint8_t shv : 1;
		uint8_t level_and_priority : 7;
	} b;
	uint8_t w;
};

/** CLIC Direct Mode mask for MTVT CSR Register */
#define CLIC_DIRECT_MODE_MTVEC_MSK   2U


#define CLIC_PENDING   (((volatile union CLICINTIP  *)(DT_REG_ADDR_BY_IDX(DT_NODELABEL(clic), 0))))
#define CLIC_IE        (((volatile union CLICINTIE *)(DT_REG_ADDR_BY_IDX(DT_NODELABEL(clic), 1))))
#define CLIC_ATTR      (((volatile union CLICINTATTR  *)(DT_REG_ADDR_BY_IDX(DT_NODELABEL(clic), 2))))
#define CLIC_CFG       (((volatile union CLICCFG  *)(DT_REG_ADDR_BY_IDX(DT_NODELABEL(clic), 3))))
#define CLIC_CTRL_SIZE (DT_REG_SIZE_BY_IDX(DT_NODELABEL(clic), 0))

/*
#ifdef CONFIG_INTERRUPTS_LEVELS
#define INTERRUPT_LEVEL CONFIG_INTERRUPTS_LEVELS
#else
#define INTERRUPT_LEVEL 0
#endif
*/
#define INTERRUPT_LEVEL CONFIG_CLIC_INT_CTRL_BITS

#define RISCV_RESERVED_INTERRUPT_BIT_MASK 24
#define RISCV_RESERVED_INTERRUPT_NUMBER   16
// #define ESWIN_CLIC_MAX_INT_CTRL_BITS      7
// #define ESWIN_CLIC_MAX_INT_LEVELS         255
#define ESWIN_CLIC_MAX_INT_CTRL_BITS      CONFIG_CLIC_MAX_INT_CTRL_BITS
#define ESWIN_CLIC_MAX_INT_LEVELS         ((CONFIG_CLIC_MAX_INT_CTRL_BITS << 5) - 1)

static uint8_t nlbits;
static uint8_t max_prio;
static uint8_t max_level;

static inline uint8_t mask8(uint8_t len)
{
	return ((1 << len) - 1) & 0xFFFFU;
}

/**
 * @brief Enable interrupt
 */
void eswin_clic_irq_enable(uint32_t irq)
{
	CLIC_IE[irq].b.IE = 1;
}

void eswin_clic_irq_pend_enable(uint32_t irq)
{
	CLIC_PENDING[irq].b.IP = 1;
}

void eswin_clic_irq_pend_disable(uint32_t irq)
{
	CLIC_PENDING[irq].b.IP = 0;
}

/**
 * @brief Disable interrupt
 */
void eswin_clic_irq_disable(uint32_t irq)
{
	CLIC_IE[irq].b.IE = 0;
}

/**
 * @brief Get enable status of interrupt
 */
int eswin_clic_irq_is_enabled(uint32_t irq)
{
	return CLIC_IE[irq].b.IE;
}

/**
 * @brief Set priority interrupt
 */
void eswin_clic_irq_set_priority(uint32_t irq, uint32_t pri)
{
	const uint8_t prio = MIN(pri, max_prio);
	const uint8_t level =  CLIC_ATTR[irq].b.level_and_priority & ~max_prio;
	const uint8_t intctrl = prio | level;

	if(nlbits == 7)
		return;

	CLIC_ATTR[irq].b.level_and_priority = intctrl;
	CLIC_ATTR[irq].b.shv = 0;
}

/**
 * @brief Set level interrupt
 */
void eswin_clic_irq_set_level(uint32_t irq, uint32_t lvl)
{
	const uint8_t prio =  CLIC_ATTR[irq].b.level_and_priority & max_prio;
	const uint8_t level = (MIN(lvl, max_level)) << (ESWIN_CLIC_MAX_INT_CTRL_BITS - nlbits);
	const uint8_t intctrl = prio | level;

	if(nlbits == 0)
		return;

	CLIC_ATTR[irq].b.level_and_priority = intctrl;
	CLIC_ATTR[irq].b.shv = 0;
}

static int eswin_clic_init(void)
{
	union CLICCFG cfg;

	cfg.b.nvbits = 0; //Currently only support clic direct mode
	cfg.b.nlbits = INTERRUPT_LEVEL;
	cfg.b.nmbits = 0;

	CLIC_CFG->w = cfg.w;

	/* Clear all pending bits and enable bits */
	for (int i = 0; i < CLIC_CTRL_SIZE; i++) {
		CLIC_PENDING[i].w = 0;
		CLIC_IE[i].w = 0;
		CLIC_ATTR[i].w = 0;
	}

	/* Support clic direct mode */
	csr_write(mtvec, ((csr_read(mtvec) & 0xFFFFFFC0) | CLIC_DIRECT_MODE_MTVEC_MSK));

	nlbits = CLIC_CFG->b.nlbits;
	max_prio = mask8(ESWIN_CLIC_MAX_INT_CTRL_BITS - nlbits);
	max_level = mask8(nlbits);;

	return 0;
}

SYS_INIT(eswin_clic_init, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
