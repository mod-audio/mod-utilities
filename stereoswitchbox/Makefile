#!/usr/bin/make -f
# Makefile for stereoswitchbox.lv2 #
# -------------------------------- #
# Created by falkTX
# Edited by Jarno
#

include ../Shared_files/Makefile.mk

# --------------------------------------------------------------

PREFIX  ?= /usr/local
DESTDIR ?=

# --------------------------------------------------------------
# Default target is to build all plugins

all: build
build: stereoswitchbox-build

# --------------------------------------------------------------
# stereoswitchbox build rules

stereoswitchbox-build: stereoswitchbox.lv2/stereoswitchbox$(LIB_EXT) stereoswitchbox.lv2/manifest.ttl

stereoswitchbox.lv2/stereoswitchbox$(LIB_EXT): stereoswitchbox.c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o $@

stereoswitchbox.lv2/manifest.ttl: stereoswitchbox.lv2/manifest.ttl
	sed -e "s|@LIB_EXT@|$(LIB_EXT)|" $< > $@

# --------------------------------------------------------------

clean:
	rm -f stereoswitchbox.lv2/stereoswitchbox$(LIB_EXT) stereoswitchbox.lv2/manifest.ttl

# --------------------------------------------------------------

install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/stereoswitchbox.lv2

	install -m 644 stereoswitchbox.lv2/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/stereoswitchbox.lv2/
	install -m 644 stereoswitchbox.lv2/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/stereoswitchbox.lv2/

# --------------------------------------------------------------
