# Copyright (c) 2022 ESWIN Co,ltd.
# SPDX-License-Identifier: Apache-2.0

zephyr_sources(soc.c)

set(SOC_LINKER_SCRIPT ${ZEPHYR_BASE}/include/zephyr/arch/riscv/common/linker.ld CACHE INTERNAL "")
