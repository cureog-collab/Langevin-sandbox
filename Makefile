CC = gcc

CFLAGS = -Wall -Wextra -I./include -O2 -MMD -MP
LDFLAGS = -lSDL2 -lm

SRC_DIR = src
OBJ_DIR = obj
BUILD_DIR = build

TARGET = $(BUILD_DIR)/Langevin-engine

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/memory.c $(SRC_DIR)/physics.c $(SRC_DIR)/render.c $(SRC_DIR)/camera_work.c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

-include $(DEPS)