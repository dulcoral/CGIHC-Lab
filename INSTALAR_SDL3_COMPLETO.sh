#!/bin/bash
# Script completo para instalar SDL3 3.3.0+ y SDL3_mixer
# Proyecto Final CGIHC 2026-1

set -e  # Detener si hay errores

echo "=========================================="
echo "Instalación Completa: SDL3 3.3.0+ y SDL3_mixer"
echo "=========================================="
echo ""

# Verificar dependencias
if ! command -v cmake &> /dev/null; then
    echo "📦 Instalando CMake..."
    brew install cmake
fi

if ! command -v git &> /dev/null; then
    echo "📦 Instalando Git..."
    brew install git
fi

if ! brew list mpg123 &> /dev/null; then
    echo "📦 Instalando mpg123 (necesario para MP3)..."
    brew install mpg123
fi

INSTALL_PREFIX="/opt/homebrew"
TEMP_DIR="$HOME/Downloads/SDL_instalacion_completa"

# Limpiar directorio temporal
if [ -d "$TEMP_DIR" ]; then
    echo "🧹 Limpiando instalación anterior..."
    rm -rf "$TEMP_DIR"
fi

mkdir -p "$TEMP_DIR"
cd "$TEMP_DIR"

# ==========================================
# PASO 1: Instalar SDL3 3.3.0+
# ==========================================
echo ""
echo "=========================================="
echo "PASO 1: Instalando SDL3 3.3.0+"
echo "=========================================="
echo ""

echo "📥 Clonando SDL3..."
git clone https://github.com/libsdl-org/SDL.git
cd SDL

echo "📌 Usando versión más reciente (main)..."
git checkout main

echo "🔨 Configurando SDL3..."
mkdir -p build
cd build

cmake .. \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_BUILD_TYPE=Release

echo "⚙️  Compilando SDL3 (esto puede tomar 5-10 minutos)..."
CPU_COUNT=$(sysctl -n hw.ncpu)
echo "   Usando $CPU_COUNT núcleos"
make -j"$CPU_COUNT"

echo "📦 Instalando SDL3..."
sudo make install

echo "✅ SDL3 instalado!"
echo ""

# Verificar versión instalada
SDL3_VERSION=$(pkg-config --modversion sdl3 2>/dev/null || echo "desconocida")
echo "   Versión instalada: $SDL3_VERSION"

# ==========================================
# PASO 2: Instalar SDL3_mixer
# ==========================================
echo ""
echo "=========================================="
echo "PASO 2: Instalando SDL3_mixer"
echo "=========================================="
echo ""

cd "$TEMP_DIR"

echo "📥 Clonando SDL3_mixer..."
git clone https://github.com/libsdl-org/SDL_mixer.git
cd SDL_mixer

echo "📌 Usando rama main..."
git checkout main

echo "🔨 Configurando SDL3_mixer..."
mkdir -p build
cd build

cmake .. \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_BUILD_TYPE=Release \
    -DSDL3_DIR="$INSTALL_PREFIX/lib/cmake/SDL3"

echo "⚙️  Compilando SDL3_mixer (esto puede tomar 3-5 minutos)..."
echo "   Usando $CPU_COUNT núcleos"
make -j"$CPU_COUNT"

echo "📦 Instalando SDL3_mixer..."
sudo make install

echo "✅ SDL3_mixer instalado!"
echo ""

# ==========================================
# Verificación Final
# ==========================================
echo ""
echo "=========================================="
echo "Verificación Final"
echo "=========================================="
echo ""

# Verificar SDL3
if [ -f "$INSTALL_PREFIX/lib/libSDL3.dylib" ] || [ -f "$INSTALL_PREFIX/lib/libSDL3.0.dylib" ]; then
    echo "✅ SDL3: Biblioteca encontrada"
    ls -lh "$INSTALL_PREFIX/lib" | grep SDL3 | grep -v mixer | head -3
else
    echo "⚠️  SDL3: Biblioteca no encontrada"
fi

# Verificar SDL3_mixer
if [ -f "$INSTALL_PREFIX/lib/libSDL3_mixer.dylib" ] || [ -f "$INSTALL_PREFIX/lib/libSDL3_mixer.0.dylib" ]; then
    echo "✅ SDL3_mixer: Biblioteca encontrada"
    ls -lh "$INSTALL_PREFIX/lib" | grep SDL3_mixer
else
    echo "⚠️  SDL3_mixer: Biblioteca no encontrada"
fi

# Verificar headers
if [ -f "$INSTALL_PREFIX/include/SDL3/SDL_mixer.h" ]; then
    echo "✅ SDL3_mixer: Header encontrado"
else
    echo "⚠️  SDL3_mixer: Header no encontrado"
fi

echo ""
echo "=========================================="
echo "✅ Instalación Completada!"
echo "=========================================="
echo ""
echo "Próximos pasos:"
echo "1. cd /Users/dulcor/Code/CGIHC-Lab"
echo "2. make clean"
echo "3. make"
echo "4. ./app"
echo ""
echo "¡Presiona Q para reproducir audio! 🎵"
echo "=========================================="

