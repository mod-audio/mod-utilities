# Please edit this to work for you

# This is how many seconds of loop memory you want your
# plugin to have available.

# comment this out if you don't want to see my debugging output
# It is actually useful to see when exactly it is changing states.

NAME = mod-note2midi
# All output is out stderr
DEBUG = -DDEBUG

# plugin name
DIR_NAME = $(shell basename "`pwd`")
PLUGIN = $(shell echo $(DIR_NAME) | tr A-Z a-z).so

# installation path
ifndef INSTALL_PATH
	INSTALL_PATH = /home/lucas/.lv2/teste.lv2/
endif

COMPLETE_INSTALL_PATH = $(INSTALL_PATH)/$(NAME).lv2

# compiler and linker
CC = g++
LD = g++

# flags
CFLAGS = -I. -I/usr/include/aubio/ -I/usr/lib/ -O2 -Wall -fPIC -DPIC -c $(DEBUG) -fpermissive
LDFLAGS = -shared -lc -lm -fPIC -DPIC -nostartfiles

# libs
# LIBS = -lm -lsamplerate
LIBS = -ljack -laubio
# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard *.cpp) 
OBJ = $(SRC:.cpp=.o)

$(PLUGIN): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PLUGIN) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) *.o $(PLUGIN)

install: $(OBJ)
	cp $(PLUGIN) $(COMPLETE_INSTALL_PATH)
	cp *.ttl $(COMPLETE_INSTALL_PATH)
	cp -r modgui $(COMPLETE_INSTALL_PATH)
