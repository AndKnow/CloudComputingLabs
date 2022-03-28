CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

src1 = ./Src/main/main.cpp ./Src/main/basic.cpp ./Src/main/advanced.cpp
src2 = $(wildcard ./Src/main/*.cpp)
src3 = $(wildcard ./Src/SudokuSoving/*.cc)
Path_Inclde = ./Include
src_test = ./Src/main/test.cpp

Target:sudoku 
test:$(src_test)
	g++ $^ -o $@

sudoku:$(src3) $(src1)
	g++ -o $@ $^ -I $(Path_Inclde) -lpthread
