#!/usr/bin/make -f
# Makefile for stereoswitchbox2.lv2 #
# --------------------------------- #
# Created by falkTX
#

include Makefile.mk

# --------------------------------------------------------------

PREFIX  ?= /usr/local
DESTDIR ?=

# --------------------------------------------------------------
# Default target is to build all plugins

all: build
build: stereoswitchbox2-build

# --------------------------------------------------------------
# stereoswitch build rules

stereoswitchbox2-build: stereoswitchbox2.lv2/stereoswitchbox2$(LIB_EXT) stereoswitchbox2.lv2/manifest.ttl

stereoswitchbox2.lv2/stereoswitchbox2$(LIB_EXT): stereoswitchbox2.c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o $@

stereoswitchbox2.lv2/manifest.ttl: stereoswitchbox2.lv2/manifest.ttl
	sed -e "s|@LIB_EXT@|$(LIB_EXT)|" $< > $@

# --------------------------------------------------------------

clean:
	rm -f stereoswitchbox2.lv2/stereoswitchbox2$(LIB_EXT) stereoswitchbox2.lv2/manifest.ttl

# --------------------------------------------------------------

install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/stereoswitchbox2.lv2

	install -m 644 stereoswitchbox2.lv2/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/stereoswitchbox2.lv2/
	install -m 644 stereoswitchbox2.lv2/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/stereoswitchbox2.lv2/

# --------------------------------------------------------------
