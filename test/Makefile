CC = gcc

INCLUDE_DIRS = $(shell find ../include ../test/include -type d)
CFLAGS = $(addprefix -I,$(INCLUDE_DIRS)) -Wall -Wextra
LDFLAGS = -lcunit

SRC_FILES = $(filter-out ../src/main.c, $(shell find ../src ../test -name '*.c'))

TARGET = test

all: execute
	@rm -f test

execute: compile
	@./test

compile:
	@gcc $(addprefix -I , $(INCLUDE_DIRS)) $(SRC_FILES) -o $(TARGET) $(LDFLAGS)

.PHONY: all