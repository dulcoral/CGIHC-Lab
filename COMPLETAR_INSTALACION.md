# Completar Instalación de Audio

Guía para completar la instalación de SDL3 en Mac.

## Paso 1: Instalar SDL3

Primero instalar SDL3.

```bash
cd ~/Downloads/SDL_instalacion_completa/SDL/build
sudo make install
```

**Nota:** Poner contraseña de Mac

---

## Paso 2: Instalar SDL3_mixer

Usar los siguientes comandos para instalar la librería de audio:

```bash
cd ~/Downloads/SDL_instalacion_completa
git clone https://github.com/libsdl-org/SDL_mixer.git
cd SDL_mixer
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/homebrew -DCMAKE_BUILD_TYPE=Release -DSDL3_DIR=/opt/homebrew/lib/cmake/SDL3
make -j$(sysctl -n hw.ncpu)
sudo make install
```

---

## ✅ Verificar

```bash
# Verificar SDL3
pkg-config --modversion sdl3
# 3.3.x o superior

# Verificar SDL3_mixer
ls /opt/homebrew/lib/libSDL3_mixer*
ls /opt/homebrew/include/SDL3/SDL_mixer.h
```

---

## 🎯 Compilar el Proyecto

Una vez todo instalado:

```bash
cd /Users/dulcor/Code/CGIHC-Lab
make clean
make
./app
```

---

