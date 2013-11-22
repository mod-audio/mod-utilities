
# installation path
ifndef INSTALL_PATH
	INSTALL_PATH = /usr/local/lib/lv2/
endif

# compiler and linker
CC = g++
LD = g++

# flags
CFLAGS = -I. -O2 -Wall -Wextra -c -fPIC -DPIC
LDFLAGS = -shared -lc -lm -fPIC -DPIC -nostartfiles

# libs
LIBS =

# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

# plugin name
PLUGIN = switchbox.so

$(PLUGIN): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PLUGIN) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) src/*.o $(PLUGIN)

install:
	mkdir -p $(INSTALL_PATH)/switchbox.lv2
	cp $(PLUGIN) $(INSTALL_PATH)/switchbox.lv2/
	cp src/*.ttl $(INSTALL_PATH)/switchbox.lv2/
	test -d src/modgui && cp -r src/modgui/ $(INSTALL_PATH)/switchbox.lv2/ || true
