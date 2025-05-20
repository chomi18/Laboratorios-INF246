
compilador = g++

flags = -std=c++17 -Wall -Wextra -Werror 
# libs = -lstdc++fs

archivo_main = LAB3_Kripper_Gonzalez.cpp
out = main

all: $(out)

$(out): $(archivo_main)
	$(compilador) $(flags) $(archivo_main) -o $(out) $(libs)

run: $(out)
	./$(out)

clean:
	rm -f $(out)
