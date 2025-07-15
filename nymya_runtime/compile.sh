#!/usr/bin/env bash
set -e

echo "Building Nymya Runtime..."
cd "$(dirname "$0")"

# Clean previous artifacts
make clean

# Build shared library
make all

echo "✔ Built $(pwd)/libnymyaruntime.so"
