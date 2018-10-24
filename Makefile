.PHONY: test all clean

ifeq ($(NDEBUG), 1)
EXTRA_FLAGS= -O3 -DNDEBUG=1
else
EXTRA_FLAGS= -fsanitize=undefined -fsanitize=address
endif

CFLAGS += \
		  -I include \
		  -I deps/bk/include \
		  -I deps \
		  -g \
		  $(EXTRA_FLAGS) \
		  -std=c99 \
		  -pedantic \
		  -Wall \
		  -Werror \
		  -Wextra \
		  -Wno-missing-field-initializers

TEST_SOURCES = $(shell find test -name '*.c')
SKL_SOURCES = $(shell find src -name '*.c')
TEST_BK_SOURCES = \
	deps/bk/src/array.c \
	deps/bk/src/default_allocator.c \
	deps/bk/src/fs/mem.c \
	deps/bk/src/printf.c \
	deps/bk/src/vendor/format/format.c


all: test

clean:
	rm -rf bin

test: bin/test
	bin/test

bin/test: $(SKL_SOURCES) $(TEST_SOURCES) $(TEST_BK_SOURCES) deps/munit/munit.c | bin
	$(CC) $(CFLAGS) $^ -o $@

bin:
	mkdir -p bin
