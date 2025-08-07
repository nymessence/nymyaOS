#!/bin/bash

# Define the names for your Docker images.
IMAGE_NAME_X86_64="nymya-build-env-x86_64"
IMAGE_NAME_ARM64="nymya-build-env-arm64"

# --- Step 1: Build the Docker Image for x86_64 ---
echo "Checking for existing Docker image: $IMAGE_NAME_X86_64..."
if docker image inspect -f '{{.Id}}' "$IMAGE_NAME_X86_64" &> /dev/null; then
  echo "Docker image found. Skipping build."
else
  echo "Docker image not found. Building now..."
  docker build -t "$IMAGE_NAME_X86_64" -f Dockerfile.x86_64 .
  if [ $? -ne 0 ]; then
    echo "Docker image build for x86_64 failed. Exiting."
    exit 1
  fi
fi

# --- Step 2: Build the Docker Image for ARM64 ---
echo "Checking for existing Docker image: $IMAGE_NAME_ARM64..."
if docker image inspect -f '{{.Id}}' "$IMAGE_NAME_ARM64" &> /dev/null; then
  echo "Docker image found. Skipping build."
else
  echo "Docker image not found. Building now..."
  docker build -t "$IMAGE_NAME_ARM64" -f Dockerfile.arm64 .
  if [ $? -ne 0 ]; then
    echo "Docker image build for ARM64 failed. Exiting."
    exit 1
  fi
fi

# --- Step 3: Run the Full Build inside the Containers ---
# The ARM64 build is run in its own container.
# The KDIR is correct for the ARM64 container as it's running on an ARM64 host.
docker run --rm \
  -v "$(pwd):/app" \
  "$IMAGE_NAME_ARM64" \
  /bin/bash -c " \
    echo 'Executing ARM64 build...' && \
    make clean && \
    make all \
  "

# The x86_64 build is run in a separate container.
# We must explicitly set the KDIR for this build to avoid using the host's kernel version.
# The command below finds the correct generic kernel headers path inside the container.
docker run --rm \
  -v "$(pwd):/app" \
  "$IMAGE_NAME_X86_64" \
  /bin/bash -c " \
    echo 'Executing x86_64 build...' && \
    export KERNEL_HEADERS_PATH=\$(ls -d /lib/modules/*/build | head -n 1) && \
    make clean && \
    make all KDIR=\${KERNEL_HEADERS_PATH} \
  "

if [ $? -ne 0 ]; then
  echo "Build process inside container failed. Check logs above."
  exit 1
fi

