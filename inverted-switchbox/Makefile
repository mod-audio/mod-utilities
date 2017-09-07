#!/usr/bin/make -f
# Makefile for inverted-switchbox.lv2 #
# ----------------------------------- #
# Created by falkTX
# Eddited by Jarno

include Makefile.mk

# --------------------------------------------------------------

PREFIX  ?= /usr/local
DESTDIR ?=

# --------------------------------------------------------------
# Default target is to build all plugins

all: build
build: inverted-switchbox-build

# --------------------------------------------------------------
# inverted-switchbox build rules

inverted-switchbox-build: inverted-switchbox.lv2/inverted-switchbox$(LIB_EXT) inverted-switchbox.lv2/manifest.ttl

inverted-switchbox.lv2/inverted-switchbox$(LIB_EXT): inverted-switchbox.c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o $@

inverted-switchbox.lv2/manifest.ttl: inverted-switchbox.lv2/manifest.ttl
	sed -e "s|@LIB_EXT@|$(LIB_EXT)|" $< > $@

# --------------------------------------------------------------

clean:
	rm -f inverted-switchbox.lv2/inverted-switchbox$(LIB_EXT) inverted-switchbox.lv2/manifest.ttl

# --------------------------------------------------------------

install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/inverted-switchbox.lv2

	install -m 644 inverted-switchbox.lv2/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/inverted-switchbox.lv2/
	install -m 644 inverted-switchbox.lv2/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/inverted-switchbox.lv2/

# --------------------------------------------------------------
