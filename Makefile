TARGET_EXEC := camover
BUILD_DIR := ./build
SRC_DIRS := ./src
CC = gcc

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CFLAGS := $(INC_FLAGS) -MMD -MP -lpthread

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)

.PHONY: install
run: $(BUILD_DIR)/$(TARGET_EXEC)
	clear
	$(BUILD_DIR)/$(TARGET_EXEC)

.PHONY: install
install: $(BUILD_DIR)/$(TARGET_EXEC)
	install -m 755 $(BUILD_DIR)/$(TARGET_EXEC) /usr/bin/

.PHONY: uninstall
uninstall:
	rm /usr/bin/$(TARGET_EXEC)