# Please edit this to work for you

# This is how many seconds of loop memory you want your
# plugin to have available.

# comment this out if you don't want to see my debugging output
# It is actually useful to see when exactly it is changing states.
# All output is out stderr

NAME = mod-peakmeter

DEBUG = -DDEBUG

# plugin name
DIR_NAME = $(shell basename "`pwd`")
PLUGIN = $(shell echo $(DIR_NAME) | tr A-Z a-z).so

# installation path
INSTALL_PATH = /usr/local/lib/lv2
COMPLETE_INSTALL_PATH = $(DESTDIR)$(INSTALL_PATH)/$(NAME).lv2

# compiler and linker
CXX ?= g++

# flags
CXXFLAGS += -std=c++11 -fPIC -c
LDFLAGS += -shared -lm

# libs
# LIBS = -lm -lsamplerate
LIBS =
# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

$(PLUGIN): $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o $(PLUGIN) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) *.o $(PLUGIN)

install: $(OBJ)
	mkdir -p $(COMPLETE_INSTALL_PATH)
	cp $(PLUGIN) $(COMPLETE_INSTALL_PATH)
	cp *.ttl $(COMPLETE_INSTALL_PATH)
