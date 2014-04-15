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
	INSTALL_PATH = /home/lucas/.lv2
endif

COMPLETE_INSTALL_PATH = $(INSTALL_PATH)/$(NAME).lv2

# compiler and linker
CC = g++
LD = g++

# flags
CFLAGS = -I. -I/usr/include/aubio -I/usr/lib/ -I/usr/lib/x86_64-linux-gnu/ -O2 -Wall -fPIC -DPIC -c $(DEBUG) -fpermissive
# CFLAGS = -I. -I/usr/include/aubio/ -I/usr/lib/ -I/usr/lib/x86_64-linux-gnu/ -O2 -Wall -fPIC -DPIC -c $(DEBUG) -fpermissive
LDFLAGS = -shared -lc -lm -fPIC -DPIC -nostartfiles

# libs
# LIBS = -lm -lsamplerate
STATIC_LIBS = -Bstatic /usr/lib/libaubio.a
DYNAMIC_LIBS = -Bdynamic -ljack
# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard *.cpp) 
OBJ = $(SRC:.cpp=.o)

$(PLUGIN): $(OBJ)
	$(LD) $(OBJ) $(STATIC_LIBS) $(DYNAMIC_LIBS) $(LDFLAGS) -o $(PLUGIN)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) *.o $(PLUGIN)

install: $(OBJ)
	mkdir -p $(COMPLETE_INSTALL_PATH)
	cp $(PLUGIN) $(COMPLETE_INSTALL_PATH)
	cp *.ttl $(COMPLETE_INSTALL_PATH)
	cp -r modgui $(COMPLETE_INSTALL_PATH)
