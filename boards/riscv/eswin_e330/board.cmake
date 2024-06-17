# Copyright (c) 2022 ESWIN Co,ltd. 
# SPDX-License-Identifier: Apache-2.0

set(QEMU_binary_suffix riscv32)
set(QEMU_CPU_TYPE_${ARCH} riscv32)

set(QEMU_FLAGS_${ARCH}
  -nographic
  -machine eswin_e330
  )

board_set_debugger_ifnset(qemu)
