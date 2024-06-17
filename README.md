programming environment setup:
    1. see: https://docs.zephyrproject.org/2.7.5/getting_started/index.html

toolchain config:
    1. $ vim ~/.bashrc
    2. add :
        export ZEPHYR_TOOLCHAIN_VARIANT=cross-compile
        # your toolchain path
        export CROSS_COMPILE=/home/user/work/zephyr/toolchain/riscv-elf-toolchain/bin/riscv64-unknown-elf-
    3. $ source ~/.bashrc

compile:
    1. $ source zephyr-env.sh
    2. cd samples/hello_world
    3. mkdir e330
    4. cd e330
    5. cmake -DBOARD=eswin_e330 .. && make

debug info:
    1. elf: ./zephyr/zephyr.elf
    2. bin: ./zephyr/zephyr.bin
