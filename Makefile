
# For compatibility with IDEs

SCONS ?= scons

all:
	$(SCONS) -j 2

clean:
	$(SCONS) -c

.PHONY: all clean

