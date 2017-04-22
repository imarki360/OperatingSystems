# g++ -Wall -Wpadded -fpack-struct=1 read.cpp

# the compiler: gcc for C program, define as g++ for C++
CC = g++

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS = -Wall -Wpadded -fpack-struct=1 -g

# build target executable
TARGET = read

all: clean $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)

run:
	./$(TARGET)
