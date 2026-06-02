# PrintBoy - Makefile pour cross-compile via aemiii91/miyoomini-toolchain.
#
# Usage local :
#   make with-toolchain        # build dans Docker (recommande)
#   make                       # build natif (necessite SDL2 + libcurl + cJSON)
#
# Le binaire produit s'appelle 'printboy' et va dans build/.

TARGET   := printboy
VERSION  := 0.2.0

SRCDIR   := src
BUILDDIR := build
SRCS     := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/screens/*.c)
OBJS     := $(SRCS:%.c=$(BUILDDIR)/%.o)

CFLAGS   ?= -O2 -Wall -Wextra -Wformat-security -Wshadow -fstack-protector-strong \
            -DVERSION=\"$(VERSION)\" -I$(SRCDIR)
LDFLAGS  ?= -lSDL2 -lSDL2_ttf -lSDL2_image -lcurl -lcjson -lm

DOCKER_IMAGE ?= aemiii91/miyoomini-toolchain:latest

.PHONY: all clean with-toolchain

all: $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

with-toolchain:
	docker run --rm -v $(CURDIR):/root/workspace $(DOCKER_IMAGE) \
		bash -lc "cd /root/workspace && make CC=arm-linux-gnueabihf-gcc"
