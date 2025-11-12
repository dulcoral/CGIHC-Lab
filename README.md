# Proyecto Final - 🏛️ Museo Virtual Interactivo 🏛️

**Curso:** Computación Gráfica e Interacción Humano Computadora  
**Grupo laboratorio:** 11  
**Grupo teoría:** 04 
**Semestre:** 2026-1  
**Autora:** Dulce Coral Rodriguez Garcia  
**No. Cuenta:** 313144545

---

## 📋 Descripción

Proyecto final que implementa un museo virtual intentando simular el museo JUMEX en 3D utilizando OpenGL. El proyecto incluye modelos 3D, animaciones por keyframes, modelos articulados, personajes animados, sistema de iluminación y audio.

## ✨ Características Principales

- **Escenario 3D completo:** Museo con 3 salas, se muestra exterior e interior
- **Sistema de iluminación avanzado:** 4 point lights + 1 spotlight + luz direccional
- **Animaciones por keyframes:** Busto de Nefertiti con movimiento simulando caer
- **Modelos articulados:** Perro con 6 partes móviles animadas (4 patas, cola y torso)
- **Personajes animados:** Modelos del servicio mixamo visto en clase, Michelle (caminando) y Joe (estático)
- **Objetos interactivos:** Tridente volador con animación de vuelo
- **Audio dinámico:** Música del museo para el interior y sonidos de ciudad para el exterior
- **Skybox:** Ambiente que complementa el escenario
- **Cámara libre:** Navegación completa WASD + mouse

## 🎮 Controles

### Navegación
- **W/A/S/D:** Movimiento de la cámara
- **Mouse:** Rotar vista
- **Scroll:** Zoom

### Vistas Predefinidas
- **E (Exterior):** Vista exterior del museo + sonido de ciudad
- **I (Interior):** Vista interior del museo + música del museo
- **Z (Zoom):** Vista de zoom a los modelos principales
- **P (Print):** Imprimir posición actual de la cámara (debug)

### Animaciones
- **0:** Activar/desactivar tridente volador
- **1:** Iniciar animación del busto
- **2:** Pausar animación del busto
- **3:** Resetear animación del busto

### Sistema
- **ESC:** Salir del programa

## 🎨 Modelos en Escena

1. **Escenario:** Museo completo con múltiples salas
2. **Busto de Nefertiti:** Con animación por keyframes (11 keyframes)
3. **David:** Escultura clásica
4. **Reliquia:** Objeto arqueológico giratorio con saltitos
5. **Totem:** Escultura tribal
6. **Estatua (Old Man):** Figura estática
7. **Escultura de Shiva:** Con tridente interactivo volador
8. **Perro:** Modelo articulado con animación de caminata (6 partes)
9. **Drone:** Animación aérea con trayectoria compleja
10. **Michelle:** Personaje animado caminando en patrón rectangular
11. **Joe:** Personaje animado estático

## 💡 Sistema de Iluminación

- **Luz Direccional:** Iluminación general ambiental
- **Point Light 0:** Luz principal general (blanca)
- **Point Light 1:** Luz tipo sol para zona de esculturas (amarillenta)
- **Point Light 2:** Luz dorada intensa para el tridente
- **Point Light 3:** Luz naranja-rojiza intensa para el adorno de Shiva
- **Spot Light 0:** Linterna de la cámara (sigue al jugador)

## 🔊 Sistema de Audio

El proyecto utiliza SDL3_mixer para audio:
- **Música del museo:** Reproducción automática al inicio (loop infinito)
- **Sonido de ciudad:** Ambiente exterior
- Los audios cambian automáticamente con las vistas E/I

## 🛠️ Requisitos del Sistema

### macOS (Desarrollo en este sistema)
- macOS 10.15 o superior
- Xcode Command Line Tools
- Homebrew

### Dependencias
```bash
# Instalar dependencias con Homebrew
brew install glfw
brew install assimp
brew install glm
brew install sdl3
brew install sdl3_mixer
```

## 📦 Instalación y Compilación

1. **Clonar el repositorio:**
```bash
git clone <URL_DEL_REPOSITORIO>
cd CGIHC-Lab
```

2. **Cambiar al branch del proyecto final:**
```bash
git checkout proyecto-final
```

3. **Compilar:**
```bash
make
```

4. **Ejecutar:**
```bash
./app
```

## 🎯 Técnicas Implementadas

- [x] Carga y renderizado de modelos 3D (OBJ, DAE)
- [x] Animación por keyframes con interpolación lineal
- [x] Modelos jerárquicos
- [x] Personajes animados
- [x] Sistema de iluminación múltiple
- [x] Texturas difusas y especulares
- [x] Skybox con cubemap
- [x] Audio 3D con SDL_mixer
- [x] Posicionamiento de la cámara
- [x] Cámara FPS con movimiento libre

## 📚 Documentación y Propuesta

- **📄 Documentación completa:** [Agregar enlace aquí]
- **📋 Propuesta del proyecto:** [Agregar enlace aquí]
- **🎥 Video demostración:** [Agregar enlace aquí]

## 📝 Notas Técnicas

- **OpenGL Version:** 3.3 Core Profile
- **GLSL Version:** 330 core
- **Formato de modelos:** OBJ (Wavefront), DAE (Collada)
- **Formato de audio:** MP3
- **Resolución:** Adaptativa (fullscreen - 80px)

## 🏆 Créditos

**Desarrollo:** Dulce Coral Rodriguez Garcia  
**Modelos 3D:** Diversos autores (ver atribuciones en documentación)  
**Audio:** Licencias libres  

---

**Universidad Nacional Autónoma de México**  
**Facultad de Ingeniería**  
2026-1
