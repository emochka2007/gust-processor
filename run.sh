#!/bin/bash
ROOT_DIR=$(pwd)
if [ -z "$1" ]; then
  echo "Usage: $0 <variable>"
  exit 1
fi
VAR=$1
cd rust-assembler
if ! cargo run "${VAR}"; then

  cd "$ROOT_DIR"
  exit 1
fi

cd $ROOT_DIR
make

echo "Running: ./emulator"
if ! ./emulator; then
  cd "$ROOT_DIR"
  exit 1
fi

cd "$ROOT_DIR"
exit 0