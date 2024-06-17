#!/bin/bash

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
    cd /home/user/work/zephyr/src/zephyr/tests/kernel/$line
    rm -r e330
    mkdir e330
    cd e330
    cmake -DBOARD=eswin_e330 ..
    make
    cd /home/user/work/zephyr/src/zephyr/tests/kernel/
    echo "compile done: $line"
  fi
done < "$filename"

echo "compile all done."