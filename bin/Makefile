# Makefile for PulseWaves command-line tools

COMPILER = clang++
COPTS = -O3 -Wall -Wno-deprecated -DNDEBUG -DUNORDERED \
	-std=c++14 -Wno-deprecated-register -Wno-writable-strings

INCLUDE = -I../inc -I../src
LIBPATH = -L../lib
LIBS = -lpulsewaves

TARGETS = pulseinfo pulse2pulse

all: $(TARGETS)

pulseinfo: pulseinfo.cpp
	$(COMPILER) $(COPTS) $(INCLUDE) $< $(LIBPATH) $(LIBS) -o $@

pulse2pulse: pulse2pulse.cpp
	$(COMPILER) $(COPTS) $(INCLUDE) $< $(LIBPATH) $(LIBS) -o $@

clean:
	rm -f $(TARGETS)

install: $(TARGETS)
	@echo "Tools built successfully!"
	@echo "Add this directory to your PATH or copy to /usr/local/bin:"
	@echo "  export PATH=\"$(PWD):\$$PATH\""

.PHONY: all clean install
