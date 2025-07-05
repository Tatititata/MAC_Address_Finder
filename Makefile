CC = gcc
LDLIBS =
CFLAGS = -Wall -Wextra -Werror -std=c11

# LDLIBS += -fsanitize=address,undefined
# CFLAGS += -fsanitize=address,undefined

OBJDIR = obj
SRC = main.c 
OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC))

TARGET = mac

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET) 

cl:
	find . -type f -name "*.c" -o -name "*.h"  -o -name "*.js" | xargs clang-format -i

cpp: 
	cppcheck --enable=all --force --error-exitcode=1 --std=c11 \
	--suppress=missingIncludeSystem --inconclusive \
	--language=c --check-level=exhaustive --verbose .

.PHONY: all clean cl valgrind cpp



