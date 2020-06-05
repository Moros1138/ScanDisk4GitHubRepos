RELEASE		:= -O3
DEBUG		:= -ggdb3 -Og
STATIC		:= -Bstatic -static-libgcc -static-libstdc++
SHARED		:=

BUILD		:= $(DEBUG)
#BUILD		:= $(RELEASE)

#LINKTYPE	:= $(STATIC)
LINKTYPE	:= $(SHARED)

CXX_FLAGS 	:= -Wall -Wextra -std=c++17 $(BUILD) -fpermissive -Wtype-limits $(LINKTYPE)
CXX			:= g++
INC_FLAG 	:= -Iinc
LIB_FLAG	:= -Llib

BIN			:= bin
SRC			:= src
INC			:= inc
LIB			:= lib
OBJ     	:= obj
RES			:= res

ifneq ($(OS),Windows_NT)
    EXECUTABLE	:= main
	CLEAN_COMMAND := -rm $(BIN)/* $(OBJ)/*.o
else
    EXECUTABLE	  := main.exe
	CLEAN_COMMAND := del $(BIN)\*.exe $(OBJ)\*.o
endif

#LIBRARIES	:= -lGL -lGLEW -lSDL2 -lSOIL -lassimp -lX11 -lpthread -lpng
LIBRARIES	:=

SOURCES		:= $(wildcard $(SRC)/*.cpp)
OBJECTS     := $(patsubst $(SRC)/%,$(OBJ)/%,$(SOURCES:.cpp=.o))

DEPSRC		:= $(wildcard $(INC)/*.hpp)
DEPENDENCIES:= $(DEPSRC:.hpp)

# if you want to find out the value of a makefile variable
# make print-VARIABLE  <--- VARIABLE is one defined here, like CXX_FLAGS, so type make print-CXX_FLAGS
print-%  : ; @echo $* = $($*)

.PHONY: depend clean all

all: $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

clean:
	$(CLEAN_COMMAND)

# Compile only
$(OBJ)/%.o : $(SRC)/%.cpp $(DEPENDENCIES)
	$(CXX) $(CXX_FLAGS) $(INC_FLAG) -c -o $@ $<

# Link the object files and libraries
$(BIN)/$(EXECUTABLE) : $(OBJECTS)
	$(CXX) $(CXX_FLAGS) -o $(BIN)/$(EXECUTABLE) $^ $(LIBRARIES) $(LIB_FLAG)
