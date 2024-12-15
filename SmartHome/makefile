CXX = g++
CXXFLAGS = -std=c++17 -Iinclude
LDFLAGS = -lsqlite3 -lssl -lcrypto -lpthread

SRC = $(wildcard DeviceManager/*.cpp) $(wildcard UserManager/*.cpp) $(wildcard DataBaseManager/*.cpp) main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
