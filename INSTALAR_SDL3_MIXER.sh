#!/bin/bash
# Script para instalar SDL3_mixer desde el código fuente
# Proyecto Final CGIHC 2026-1

set -e  # Detener si hay errores

echo "=========================================="
echo "Instalación de SDL3_mixer desde código fuente"
echo "=========================================="
echo ""

# Verificar que CMake está instalado
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake no está instalado. Instalando..."
    brew install cmake
fi

# Verificar que git está instalado
if ! command -v git &> /dev/null; then
    echo "❌ Git no está instalado. Instalando..."
    brew install git
fi

# Verificar que mpg123 está instalado (necesario para soporte MP3)
if ! brew list mpg123 &> /dev/null; then
    echo "📦 Instalando mpg123 (necesario para soporte MP3)..."
    brew install mpg123
fi

# Directorio temporal
TEMP_DIR="$HOME/Downloads/SDL_mixer_build"
INSTALL_PREFIX="/opt/homebrew"

echo "📥 Clonando repositorio SDL_mixer..."
if [ -d "$TEMP_DIR" ]; then
    echo "⚠️  El directorio $TEMP_DIR ya existe. Eliminando..."
    rm -rf "$TEMP_DIR"
fi

mkdir -p "$TEMP_DIR"
cd "$TEMP_DIR"

# Clonar el repositorio
git clone https://github.com/libsdl-org/SDL_mixer.git
cd SDL_mixer

# Usar una versión compatible con SDL3 3.2.x
# Intentar usar la rama main que suele ser más flexible
echo "📌 Usando rama main (compatible con SDL3 3.2.x)..."
git checkout main 2>/dev/null || echo "Ya en rama main"

# Modificar temporalmente CMakeLists.txt para aceptar SDL3 3.2.x
echo "🔧 Ajustando requisitos de versión de SDL3..."
if [ -f "CMakeLists.txt" ]; then
    # Crear backup
    cp CMakeLists.txt CMakeLists.txt.backup
    
    # Modificar SDL_REQUIRED_VERSION de 3.3.0 a 3.2.0
    sed -i '' 's/set(SDL_REQUIRED_VERSION 3.3.0)/set(SDL_REQUIRED_VERSION 3.2.0)/g' CMakeLists.txt 2>/dev/null || \
    sed -i 's/set(SDL_REQUIRED_VERSION 3.3.0)/set(SDL_REQUIRED_VERSION 3.2.0)/g' CMakeLists.txt
    
    echo "✅ CMakeLists.txt ajustado para SDL3 3.2.x"
    echo "   Cambiado: SDL_REQUIRED_VERSION de 3.3.0 a 3.2.0"
fi

echo ""
echo "🔨 Configurando con CMake..."
mkdir -p build
cd build

# Configurar con CMake - permitir versiones menores de SDL3
cmake .. \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_BUILD_TYPE=Release \
    -DSDL3MIXER_MP3_MPG123=ON \
    -DSDL3MIXER_MP3_DRMP3=OFF \
    -DSDL3MIXER_OPUS=OFF \
    -DSDL3MIXER_FLAC=OFF \
    -DSDL3MIXER_MOD=OFF \
    -DSDL3MIXER_MIDI=OFF \
    -DSDL3_DIR=/opt/homebrew/lib/cmake/SDL3

echo ""
echo "⚙️  Compilando (esto puede tomar varios minutos)..."
CPU_COUNT=$(sysctl -n hw.ncpu)
echo "   Usando $CPU_COUNT núcleos para compilar"
make -j"$CPU_COUNT"

echo ""
echo "📦 Instalando en $INSTALL_PREFIX..."
sudo make install

echo ""
echo "✅ Instalación completada!"
echo ""
echo "🔍 Verificando instalación..."

# Verificar que los archivos se instalaron
if [ -f "$INSTALL_PREFIX/lib/libSDL3_mixer.dylib" ] || [ -f "$INSTALL_PREFIX/lib/libSDL3_mixer.0.dylib" ]; then
    echo "✅ Biblioteca encontrada en $INSTALL_PREFIX/lib"
    ls -lh "$INSTALL_PREFIX/lib" | grep SDL3_mixer
else
    echo "⚠️  No se encontró la biblioteca. Verifica los logs arriba."
fi

if [ -f "$INSTALL_PREFIX/include/SDL3/SDL_mixer.h" ]; then
    echo "✅ Header encontrado en $INSTALL_PREFIX/include/SDL3/"
else
    echo "⚠️  No se encontró el header. Verifica los logs arriba."
fi

echo ""
echo "=========================================="
echo "Próximos pasos:"
echo "1. cd /Users/dulcor/Code/CGIHC-Lab"
echo "2. make clean"
echo "3. make"
echo "4. ./app"
echo "=========================================="

