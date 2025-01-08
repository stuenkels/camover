TARGET_EXEC := tcpmap
BUILD_DIR := ./build
SRC_DIRS := ./src
CC = gcc

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS := $(INC_FLAGS) -MMD -MP -lpthread

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)

run: $(BUILD_DIR)/$(TARGET_EXEC)
	clear
	$(BUILD_DIR)/$(TARGET_EXEC)

install: $(BUILD_DIR)/$(TARGET_EXEC)
	install -m 755 $(BUILD_DIR)/$(TARGET_EXEC) /usr/bin/