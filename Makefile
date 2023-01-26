VERSION 	=3
CXX			= g++
DEBUG 		= -g
CXXFLAGS	= -Wall -Wextra -Wpedantic
SOURCES	 	= $(wildcard ./code/Src/Stack/*.cpp) $(wildcard ./code/Src/Onegin/*.cpp)
INCLUDES  	= -I ./code/Inc/Onegin -I ./code/Inc/Stack -I ./code/Inc
LIB_NAMES 	=
LIB_PATH 	= -lm
OBJ			=$(patsubst %.cpp, %.o, $(SOURCES))

#links
CPU_$(VERSION): $(OBJ) ./code/Src/CPU.o
	@mkdir -p progect
	$(CXX) $(OBJ) ./code/Src/CPU.o $(LIB_PATH) $(LIB_NAMES) -o ./progect/CPU_$(VERSION)
	@rm -rf $(OBJ) ./code/Src/CPU.o
	
ASM_$(VERSION): $(OBJ) ./code/Src/ASM.o
	@mkdir -p progect
	$(CXX) $(OBJ) ./code/Src/ASM.o $(LIB_PATH) $(LIB_NAMES) -o ./progect/ASM_$(VERSION)
	@rm -rf $(OBJ) ./code/Src/ASM.o

#compile
%.o: %.cpp
	$(CXX) $(INCLUDES) $(DEBUG) -c $(CXXFLAGS) $< -o $@


program.bin: ASM_$(VERSION)
	./progect/ASM_$(VERSION) program.asm program.bin

.PHONY:start
start: 
	./progect/CPU_$(VERSION) program.bin

.PHONY:clear
clear:
	@echo "Remove linked and compiled files......"
	rm -rf $(OBJ) $(TARGET) progect 
