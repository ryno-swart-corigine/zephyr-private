cd ~/qian/sdk/zephyr-test/kernel/$1
cp ~/qian/sdk/zephyr-test/kernel/run.gdb .
/home/mimic/qian/sdk/zephyr/toolchain/riscv-elf-toolchain/bin/riscv-unknown-elf-gdb -x run.gdb

