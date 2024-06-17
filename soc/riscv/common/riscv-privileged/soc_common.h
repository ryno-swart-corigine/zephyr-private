/*
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file configuration macros for riscv SOCs supporting the riscv
 *       privileged architecture specification
 */

#ifndef __SOC_COMMON_H_
#define __SOC_COMMON_H_

/* IRQ numbers */
#define RISCV_MACHINE_SOFT_IRQ       3  /* Machine Software Interrupt */
#define RISCV_MACHINE_EXT_IRQ        11 /* Machine External Interrupt */

/* ECALL Exception numbers */
#define SOC_MCAUSE_ECALL_EXP         11 /* Machine ECALL instruction */
#define SOC_MCAUSE_USER_ECALL_EXP    8  /* User ECALL instruction */

/* SOC-specific MCAUSE bitfields */
#ifdef CONFIG_64BIT
/* Interrupt Mask */
#define SOC_MCAUSE_IRQ_MASK          (1 << 63)
#else
/* Interrupt Mask */
#define SOC_MCAUSE_IRQ_MASK          (1 << 31)
#endif

/* Exception code Mask */
#define SOC_MCAUSE_EXP_MASK          CONFIG_RISCV_MCAUSE_EXCEPTION_MASK

#ifndef _ASMLANGUAGE

#include <zephyr/drivers/interrupt_controller/riscv_clic.h>
#include <zephyr/drivers/interrupt_controller/riscv_plic.h>

#if defined(CONFIG_RISCV_SOC_INTERRUPT_INIT)
void soc_interrupt_init(void);
#endif

#if defined(CONFIG_ESWIN_CLIC)
void eswin_clic_irq_enable(uint32_t irq);
void eswin_clic_irq_disable(uint32_t irq);
void eswin_clic_irq_pend_disable(uint32_t irq);
void eswin_clic_irq_pend_enable(uint32_t irq);
int  eswin_clic_irq_is_enabled(uint32_t irq);
void eswin_clic_irq_set_priority(uint32_t irq, uint32_t pri);
void eswin_clic_irq_set_level(uint32_t irq, uint32_t lvl);
#endif

#endif /* !_ASMLANGUAGE */

#endif /* __SOC_COMMON_H_ */
