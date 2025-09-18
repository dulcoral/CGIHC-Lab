# Makefile para macOS (Apple Silicon/Intel) usando Homebrew y pkg-config
# Requisitos:
#   brew install glfw assimp sdl3 pkg-config
#   Opcional: glad ya está en el repo (glad.c)

PROJECT_ROOT := $(abspath .)
TARGET       := app

# Configuración de build: DEBUG=1 para modo debug
DEBUG ?= 0

CXX          := clang++
CC           := clang

SRC_CPP      := Main.cpp
SRC_C        := glad.c

INCLUDE_DIRS := $(PKG_SDL3_CFLAGS) -I$(PROJECT_ROOT)/include -I$(PROJECT_ROOT)/include/glad -I$(PROJECT_ROOT)

# Detección de Homebrew y pkg-config
BREW_PREFIX  := $(shell brew --prefix 2>/dev/null)
export PKG_CONFIG_PATH ?= $(BREW_PREFIX)/lib/pkgconfig

# Librerías vía pkg-config (si existen); si no, usamos fallback con -I/-L
PKG_GLFW_LIBS   := $(shell pkg-config --libs glfw3 2>/dev/null)
PKG_GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
PKG_ASSIMP_LIBS := $(shell pkg-config --libs assimp 2>/dev/null)
PKG_ASSIMP_CFLAGS := $(shell pkg-config --cflags assimp 2>/dev/null)
PKG_SDL3_LIBS   := $(shell pkg-config --libs sdl3 2>/dev/null)
PKG_SDL3_CFLAGS := $(shell pkg-config --cflags sdl3 2>/dev/null)

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
FALLBACK_CFLAGS := -I$(BREW_PREFIX)/include
FALLBACK_LIBS   := -L$(BREW_PREFIX)/lib -lglfw -lassimp
PLATFORM_GL_LIBS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else ifeq ($(UNAME_S),Linux)
FALLBACK_CFLAGS :=
FALLBACK_LIBS   := -lglfw -lassimp -lGL -ldl -lpthread -lX11 -lXrandr -lXi -lXxf86vm -lXcursor
PLATFORM_GL_LIBS := -lGL
else
FALLBACK_CFLAGS :=
FALLBACK_LIBS   :=
PLATFORM_GL_LIBS :=
endif

ALL_PKG_CFLAGS := $(PKG_GLFW_CFLAGS) $(PKG_ASSIMP_CFLAGS) $(PKG_SDL3_CFLAGS)
ALL_PKG_LIBS   := $(PKG_GLFW_LIBS) $(PKG_ASSIMP_LIBS) $(PKG_SDL3_LIBS)

# GL libs por plataforma definidos en PLATFORM_GL_LIBS arriba

CXXFLAGS := -std=c++17 -DGL_SILENCE_DEPRECATION $(INCLUDE_DIRS) $(or $(strip $(ALL_PKG_CFLAGS)),$(FALLBACK_CFLAGS))
CFLAGS   := -DGL_SILENCE_DEPRECATION $(INCLUDE_DIRS) $(or $(strip $(ALL_PKG_CFLAGS)),$(FALLBACK_CFLAGS))

ifeq ($(DEBUG),1)
  CXXFLAGS += -O0 -g -DDEBUG
  CFLAGS   += -O0 -g -DDEBUG
else
  CXXFLAGS += -O2
  CFLAGS   += -O2
endif
LDFLAGS  := $(PLATFORM_GL_LIBS) $(or $(strip $(ALL_PKG_LIBS)),$(FALLBACK_LIBS)) -Wl,-rpath,$(BREW_PREFIX)/lib

OBJS := $(SRC_CPP:.cpp=.o) $(SRC_C:.c=.o)

.PHONY: all run clean deps install-deps help debug release

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

run-debug: $(TARGET)
	@echo "Running under LLDB (batch). If it crashes, backtrace will be printed."
	@lldb --batch -- ./$(TARGET) -o "run" -o "bt all" -o "quit" | cat

run-interrupt: $(TARGET)
	@echo "Running under LLDB, interrupting after start to capture stacks."
	@lldb --batch -- ./$(TARGET) -o "run" -o "process interrupt" -o "bt all" -o "quit" | cat

clean:
	rm -f $(OBJS) $(TARGET)

# Comprobación de dependencias de Homebrew
deps:
	@command -v brew >/dev/null 2>&1 || { echo "brew no está instalado: https://brew.sh"; exit 1; }
	@for p in glfw assimp sdl3 pkg-config; do \
	  brew list $$p >/dev/null 2>&1 || echo "Falta paquete Homebrew: $$p"; \
	done

# Instalar dependencias con Homebrew (no solo listar)
install-deps:
	@command -v brew >/dev/null 2>&1 || { echo "brew no está instalado: https://brew.sh"; exit 1; }
	brew update || true
	brew install pkg-config glfw assimp sdl3 || true

# Atajos de configuración
debug:
	$(MAKE) DEBUG=1 all

release:
	$(MAKE) DEBUG=0 all

# Ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make             - Compilar en modo release (DEFAULT)"
	@echo "  make debug       - Compilar en modo debug (-O0 -g)"
	@echo "  make release     - Compilar en modo release (-O2)"
	@echo "  make run         - Ejecutar ./$(TARGET)"
	@echo "  make run-debug   - Ejecutar bajo LLDB (batch) y mostrar backtrace si falla"
	@echo "  make run-interrupt- Ejecutar bajo LLDB, interrumpir y mostrar stacks"
	@echo "  make clean       - Limpiar binarios y objetos"
	@echo "  make deps        - Verificar dependencias de Homebrew"
	@echo "  make install-deps- Instalar dependencias con Homebrew"

