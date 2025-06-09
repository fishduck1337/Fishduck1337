COMPILE_ARGS = -std=c++17
SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET = main
CC = g++

run: $(TARGET)
	./$(TARGET)
$(TARGET) : $(OBJS)
	$(CC) $(COMPILE_ARGS) -o $@ $^
$(OBJS) : %.o : %.cpp
	$(CC) $(COMPILE_ARGS) -c $< -o $@
all: $(TARGET)
rebuild: clean all

.phony: clean all rebuild
clean:
	rm -rf $(OBJS) $(TARGET)

.phony: clean
