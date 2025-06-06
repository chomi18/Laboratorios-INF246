# Compiladores
compilador_cpp = g++
compilador_java = javac

# Flags C++
flags = -std=c++17 -Wall -Wextra -Werror
libs = -lstdc++fs

# Archivos
archivo_main_cpp = LAB3_Kripper_Gonzalez.cpp
archivo_main_java = LAB3_Kripper_Gonzalez.java
out_cpp = main
out_java_class = LAB3_Kripper_Gonzalez.class

# Targets
all: $(out_cpp) $(out_java_class)

$(out_cpp): $(archivo_main_cpp)
	$(compilador_cpp) $(flags) $(archivo_main_cpp) -o $(out_cpp) $(libs)

$(out_java_class): $(archivo_main_java)
	$(compilador_java) $(archivo_main_java)

run: $(out_cpp) $(out_java_class)
	@echo "=== Ejecutando programa C++ ==="
	./$(out_cpp)
	@echo ""
	@echo "=== Ejecutando programa Java ==="
	java LAB3_Kripper_Gonzalez

clear:
	rm -f $(out_cpp) $(out_java_class)

