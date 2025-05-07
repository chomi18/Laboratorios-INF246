compilador = g++

flags = -std=c++17 -Wall -Wextra -Werror -g

archivo_main = LAB2_Kripper_Gonzalez.cpp
out = blackjack

all: $(out)

$(out): $(archivo_main)
	$(compilador) $(flags) $(archivo_main) -o $(out)

run: $(out)
	./$(out)

valgrind: $(out)
	valgrind --leak-check=full --track-origins=yes ./$(out)

clean:
	rm -f $(out)