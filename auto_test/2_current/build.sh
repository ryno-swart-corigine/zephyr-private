#!/bin/bash
cd /home/user/work/zephyr/src/zephyr/tests/kernel/$1
mkdir e330
cd e330
cmake -DBOARD=eswin_e330 ..
make
cd /home/user/work/zephyr/src/zephyr/tests/kernel/
dir_name=$1
./scp.sh "$dir_name"
