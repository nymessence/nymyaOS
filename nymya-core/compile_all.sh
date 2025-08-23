#!/bin/bash
set -euo pipefail

echo "🚀 Starting all builds..."

./compile_arm64_raspi.sh
./compile_arm64_generic.sh
./compile_x86_64.sh
./compile_riscv64.sh

echo "🎉 All builds completed."