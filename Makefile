CC = g++
INCLUDES = -I/home/anl/include
FLAGS = -Wall -g -no-pie -m64 -std=c++17 -lstdc++fs
SRC_DIR = src
OBJ_DIR = obj
LIBS = -lstdc++ -lstdc++fs -lGLEW -lGL -lglfw3 -lX11 -lpthread -ldl
TARGET = main
DEFINES = FILESYSTEM_EXPERIMENTAL
		
ifeq ($(INCLUDES), NONE)
	CXXFLAGS = $(FLAGS)
else
	CXXFLAGS = $(FLAGS) $(INCLUDES)
endif
SRC = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/*.hpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEFINES_FLAGS = $(patsubst %,-D %,$(DEFINES))

default: $(TARGET).out

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CC) -c -o  $@ $< $(CXXFLAGS) $(DEFINES_FLAGS)

$(TARGET).out: $(OBJS)
	g++ $^  -o  $@   $(FLAGS) $(LIBS)

clean:
	rm $(OBJ_DIR)/*.o *.out

run:
	./$(TARGET).out
