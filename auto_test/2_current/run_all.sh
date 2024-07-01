# !/bin/bash

if [ $# -eq 0 ]; then
  echo "Usage: $0 filename"
  exit 1
fi

filename=$1

if [ ! -f "$filename" ]; then
  echo "Error: File '$filename' not found."
  exit 1
fi

while IFS= read -r line; do
  if [ -n "$line" ]; then
    echo "Starting test: $line"
    start_time=$SECONDS
    gnome-terminal --maximize --title "kernel-test" -- bash -c "cd /home/user/work/zephyr/src/zephyr/tests/kernel/$line/e330/zephyr;sleep 1;riscv64-unknown-elf-gdb -x /home/user/work/zephyr/src/zephyr/auto_test/2_current/run.gdb"


    sleep 10
    while true; do
        last_line=$(tail -n 1 "/home/user/work/zephyr/src/zephyr/auto_test/2_current/test.log")
        fail_line="PROJECT EXECUTION FAILED"
        sucess_line="PROJECT EXECUTION SUCCESSFUL"
        if [[ *"$last_line"* == *"$fail_line"* || *"$last_line"* == *"$sucess_line"* ]]; then
            break
        fi
        end_time=$SECONDS
        run_time=$((end_time - start_time))
        if [ $run_time -gt 180 ]; then
            echo "timeout test: $line"
            break
        fi
    done
    # kill -9 $gdb_pid
    xdotool search --name "kernel-test" windowclose
    echo "End test: $line"
  fi
done < "$filename"
