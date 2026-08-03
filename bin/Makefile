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
	@echo "Tools built successfully in bin/ directory!"
	@echo "Add bin/ to your PATH or create symlinks:"
	@echo "  export PATH=\"\$$PATH:$(PWD)\""
	@echo "  # Or: sudo ln -s $(PWD)/pulseinfo /usr/local/bin/pulseinfo"
	@echo "  # Or: sudo ln -s $(PWD)/pulse2pulse /usr/local/bin/pulse2pulse"

.PHONY: all clean install
