CXX = clang++
APP_NAME = chirpc

CXX_FLAGS =-std=c++17 $(FLAGS)
SRC_ROOT =src
DEST_DIR =bin

.PHONY : all setup

#Create the include dirs in a list with -I in front of each item
INCLUDES = $(patsubst %, -I%, $(shell find $(SRC_ROOT) -type d))

#Set VPATH to find the source files
VPATH = $(shell find $(SRC_ROOT) -type d)

#Just get the filenames
SOURCE_FILES=$(notdir $(shell find $(SRC_ROOT) -name *.cpp))

#Turn the file names into object filenames
OBJECTS = $(SOURCE_FILES:%.cpp=$(DEST_DIR)/%.o)

.ONESHELL:
all: $(APP_NAME)

$(APP_NAME) : $(OBJECTS)
	$(CXX) -o $(APP_NAME) $^ $(CXX_FLAGS)	

setup:
	@echo "Making a build folder"
	@mkdir -p $(DEST_DIR)

$(DEST_DIR)/%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXX_FLAGS) $(INCLUDES)

clean:
	rm $(APP_NAME)
	rm $(DEST_DIR)/*.o