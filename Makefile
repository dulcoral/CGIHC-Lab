# Makefile para proyecto OpenGL en macOS
# Compilador
CXX = clang++

# Flags de compilación
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Rutas de include
INCLUDES = -I/opt/homebrew/opt/glfw/include \
           -I/opt/homebrew/opt/glew/include \
           -Iinclude_local

# Rutas de bibliotecas
LIBDIRS = -L/opt/homebrew/opt/glfw/lib \
          -L/opt/homebrew/opt/glew/lib

# Bibliotecas a enlazar
LIBS = -lglfw -lGLEW -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Archivos fuente
SRCDIR = configInicial
SOURCES = $(SRCDIR)/Main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Nombre del ejecutable
TARGET = opengl_app

# Regla principal
all: $(TARGET)

# Compilar ejecutable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

# Compilar objetos
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET)

# Limpiar archivos compilados
clean:
	rm -f $(OBJECTS) $(TARGET)

# Instalar dependencias (si no están instaladas)
install-deps:
	brew install glfw glew

# Mostrar ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make         - Compilar el proyecto"
	@echo "  make run     - Compilar y ejecutar el proyecto"
	@echo "  make clean   - Limpiar archivos compilados"
	@echo "  make install-deps - Instalar dependencias"
	@echo "  make help    - Mostrar esta ayuda"

.PHONY: all run clean install-deps help
