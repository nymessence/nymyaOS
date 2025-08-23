# === Nymya Runtime Makefile ===

include ../version.conf

CC           = gcc
CFLAGS       = -Wall -O2 -fPIC
LDFLAGS      = -shared

# Where to find the core header
INCLUDE_DIR  = ../nymya-core/*.h

SRC_DIR      = .
OBJ_DIR      = build
LIB_NAME     = nymyaruntime
LIB_FILE     = lib$(LIB_NAME).so

# Runtime sources
SOURCES      = nymya_runtime.c backend_sim.c backend_qpu.c
OBJECTS      = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SOURCES))

.PHONY: all clean install

all: $(LIB_FILE)

# Compile each .c into build/*.o
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link shared library
$(LIB_FILE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

# Install to system (optional)
install: all
	@echo "Installing $(LIB_FILE) to $(DESTDIR)/usr/lib"
	mkdir -p $(DESTDIR)/usr/lib
	cp $(LIB_FILE) $(DESTDIR)/usr/lib/
	@echo "Done."

clean:
	rm -rf $(OBJ_DIR) $(LIB_FILE)
