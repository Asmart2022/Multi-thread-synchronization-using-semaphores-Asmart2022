# Makefile for cse4001_sync

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -O2
TARGET   = cse4001_sync
SRCS     = cse4001_sync.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
