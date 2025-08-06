# --- Import package metadata ---
-include ../version.conf

VERSION       ?= 0.1.0
PKG_ARCH      ?= amd64
MAINTAINER    ?= Unknown <>
DESCRIPTION   := NymyaOS Core Syscall SDK

# --- Toolchain & flags ---
CC             := gcc
KERNEL_BUILD_DIR := /lib/modules/$(shell uname -r)/build
CFLAGS         := -Wall -fPIC -O2
LDFLAGS        := -shared

# --- Supported architectures ---
ARCHS          := arm64 x86_64

# --- Cross-compilers per arch ---
CROSS_COMPILE_arm64   := aarch64-linux-gnu-
CROSS_COMPILE_x86_64  :=

# --- Paths & names ---
INCLUDE_DIR    := .
SRC_DIR        := .
KERNEL_SRC_DIR := kernel_syscalls
OBJ_DIR        := build
LIB_NAME       := nymya
LIB_FILE       := lib$(LIB_NAME).so
KERNEL_MODULE  := nymya_syscalls.ko

INSTALL_LIB_DIR     := /usr/lib
INSTALL_INCLUDE_DIR := /usr/include/$(LIB_NAME)
INSTALL_KERNEL_DIR  := /lib/modules/$(shell uname -r)/extra

# --- .deb output dirs template ---
DEB_USERLAND_DIR := nymya-core-userland_$(VERSION)_$(PKG_ARCH)
DEB_KERNEL_DIR   := nymya-core-kernel_$(VERSION)_#ARCH#

# --- Userland sources & objects ---
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# -----------------------------------------------------------------------------
# Default: build userland library
# -----------------------------------------------------------------------------
.PHONY: all userland
all: userland

userland: $(LIB_FILE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(LIB_FILE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# -----------------------------------------------------------------------------
# Kernel module sources & objects
# -----------------------------------------------------------------------------
# Explicitly list all C files that constitute the nymya_syscalls.ko module.
KERNEL_MOD_C_FILES := \
	nymya_syscalls.c \
	complex_mul.c \
	complex_exp_i.c \
	fixed_point_square.c \
	fixed_point_mul.c \
	fixed_sin.c \
	make_complex.c \
	nymya_3303_pauli_x.c \
	fixed_complex_multiply.c \
	nymya_exit_syscall_print_funcs.c \
	nymya_event_class_syscall_enter.c \
	nymya_enter_syscall_print_funcs.c \
	nymya_event_class_syscall_exit.c \
	log.c \
	nymya_complex_math.c

# Full paths for kernel module source files
KERNEL_SRCS := $(addprefix $(KERNEL_SRC_DIR)/,$(KERNEL_MOD_C_FILES))

# Kernel object filenames for inner Makefile
KERNEL_OBJS := $(patsubst %.c,%.o,$(KERNEL_MOD_C_FILES))

# -----------------------------
