#!/usr/bin/expect
set dir_name [lindex $argv 0]
set password "fpga4risc5"
spawn scp /home/user/work/zephyr/src/zephyr/tests/kernel/$dir_name/e330/zephyr/zephyr.elf /home/user/work/zephyr/src/zephyr/tests/kernel/$dir_name/e330/zephyr/zephyr.bin  fpga@10.12.208.106:/rv/fpga/e0006758/sdk/zephyr-test/kernel/$dir_name
expect "password:"
send "$password\r"
expect eof
