/*---------------------------------------------------------*/
/* ----------------   Proyecto Final --------------------------*/
/*-----------------    2026-1   ---------------------------*/
/*------------- Alumno: Dulce Coral Rodriguez Garcia    ---------------*/
/*------------- No. Cuenta 313144545     ---------------*/
/*---------------------------------------------------------*/
/*
 * CONTROLES:
 * 
 * NAVEGACIÓN:
 * - WASD: Movimiento libre de la cámara
 * - Mouse: Rotar cámara
 * - Scroll: Zoom
 * 
 * VISTAS PREDEFINIDAS:
 * - E (Exterior): Vista exterior del museo + sonido de ciudad
 * - I (Interior): Vista interior del museo + música del museo
 * - Z (Zoom): Vista de zoom a los modelos principales
 * - R (Reliquia): Vista de la reliquia
 * - P (Print): Imprimir posición actual de la cámara
 * 
 * AUDIO:
 * - La música del museo se reproduce automáticamente al inicio (museo_music.mp3)
 * - Use E para cambiar a sonido de ciudad (sonido_ciudad.mp3)
 * - Use I para cambiar a música del museo (museo_music.mp3)
 * 
 * ANIMACIONES Y EFECTOS ESPECIALES:
 * - 0: Tridente volador
 * - 1: Iniciar animación del busto
 * - 2: Pausar animación del busto
 * - 3: Resetear animación del busto
 * 
 * - ESC: Salir del programa
 * 
 * MODELOS EN ESCENA:
 * - Escenario: Museo completo
 * - Drone: Animación aérea en movimiento
 * - Busto de Nefertiti: Con animación por keyframes
 * - David: Escultura clásica
 * - Reliquia: Objeto arqueológico giratorio con saltitos
 * - Totem: Escultura tribal
 * - Estatua (Old Man): Figura humana
 * - Escultura (Shiva): Con tridente interactivo
 * - Perro: Modelo articulado con animación
 * - Michelle: Personaje animado caminando
 * - Joe: Personaje animado estático
 */
/*---------------------------------------------------------*/

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>					//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>

struct DroneAnimation;
struct BustoAnimation;

namespace DroneAnimConfig {
	constexpr float ANGLE_INCREMENT = 0.01f;
	constexpr float ROTATION_INCREMENT = 0.002f;
}

namespace BustoAnimConfig {
	constexpr float INITIAL_HEIGHT = 155.0f;
	constexpr float FINAL_HEIGHT = 30.0f;
	constexpr float BASE_X = 20.0f;
	constexpr float BASE_Z = 150.0f;
}

namespace BustoRenderConfig {
	const glm::vec3 BASE_POS = glm::vec3(-50.0f, 0.0f, 150.0f);
	const glm::vec3 PIVOT_OFFSET = glm::vec3(300.0f, -5.0f, -600.0f);
	constexpr float SCALE = 200.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);
void processInput(GLFWwindow* window);

// Funciones de animación y renderizado del Drone
glm::vec3 calculateDronePosition(const DroneAnimation& anim);
void updateDroneAnimation(DroneAnimation& anim);
void renderDrone(Model& droneModel, Shader& shader, const DroneAnimation& anim);

// Funciones de animación y renderizado del Busto
void initBustoKeyframes(BustoAnimation& anim);
void updateBustoAnimation(BustoAnimation& anim);
void playBustoAnimation(BustoAnimation& anim);
void pauseBustoAnimation(BustoAnimation& anim);
void resetBustoAnimation(BustoAnimation& anim);
void renderBusto(Model& bustoModel, Shader& shader, const BustoAnimation& anim);

// Funciones de renderizado de modelos estáticos del museo
void renderdavid(Model& davidModel, Shader& shader);
void renderReliquia(Model& reliquiaModel, Shader& shader);
void renderTotem(Model& totemModel, Shader& shader);
void renderEstatua(Model& estatuaModel, Shader& shader);
void renderEscultura(Model& base, Model& tridente, Model& adorno, Shader& shader);

// Funcion de renderizado del Perro (modelo articulado)
void renderPerro(Model& cuerpo, Model& cola, Model& pataDerDel, Model& pataIzqDel, 
			     Model& pataDerTra, Model& pataIzqTra, Shader& shader);

// setup point lights
void setupPointLight(Shader& shader, int index, const glm::vec3& position, 
                     const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
                     float constant, float linear, float quadratic)
{
	std::string base = "pointLight[" + std::to_string(index) + "].";
	shader.setVec3(base + "position", position);
	shader.setVec3(base + "ambient", ambient);
	shader.setVec3(base + "diffuse", diffuse);
	shader.setVec3(base + "specular", specular);
	shader.setFloat(base + "constant", constant);
	shader.setFloat(base + "linear", linear);
	shader.setFloat(base + "quadratic", quadratic);
}

// setup spot light
void setupSpotLight(Shader& shader, int index, const glm::vec3& position, const glm::vec3& direction,
                    const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
                    float cutOff, float outerCutOff, float constant, float linear, float quadratic)
{
	std::string base = "spotLight[" + std::to_string(index) + "].";
	shader.setVec3(base + "position", position);
	shader.setVec3(base + "direction", direction);
	shader.setVec3(base + "ambient", ambient);
	shader.setVec3(base + "diffuse", diffuse);
	shader.setVec3(base + "specular", specular);
	shader.setFloat(base + "cutOff", cutOff);
	shader.setFloat(base + "outerCutOff", outerCutOff);
	shader.setFloat(base + "constant", constant);
	shader.setFloat(base + "linear", linear);
	shader.setFloat(base + "quadratic", quadratic);
}

// GLFW error callback to diagnose initialization issues on macOS
static void glfw_error_callback(int error, const char* description)
{
	std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;


//Camera
Camera camera(glm::vec3(0.0f, 200.0f, 800.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
		lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 5.0f,
lastFrame = 0.0f;

void getResolution(void);

//Lighting
glm::vec3 lightDirection(-1.0f, 0.0f, 0.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// (Removed unused car animation variables)

// Animación de Michelle
enum MichelleState {
	WALKING_RIGHT,
	TURNING_1,
	WALKING_FORWARD,
	TURNING_2,
	WALKING_FINAL,
	TURNING_180,
	WALKING_BACK_1,
	TURNING_BACK_1,
	WALKING_BACK_2,
	TURNING_BACK_2,
	WALKING_LEFT,
	TURNING_FINAL
};
// CONSTANTES DE ANIMACIÓN
// Michelle
constexpr float MICHELLE_SPEED = 2.0f;
constexpr float MICHELLE_RANGE_MAX = 650.0f;
constexpr float MICHELLE_WALK_DISTANCE = 200.0f;
constexpr float MICHELLE_FINAL_WALK = 100.0f;
constexpr float MICHELLE_ROTATION_SPEED = 2.0f;
constexpr float MICHELLE_INITIAL_ROTATION = 70.0f;

// Perro - Constantes
constexpr float PERRO_ANIM_SPEED = 1.0f;
constexpr float PERRO_PATA_MAX = 3.5f;
constexpr float PERRO_MOV_SPEED = 1.5f;
constexpr float PERRO_MOV_RANGE = 600.0f;
constexpr float PERRO_INITIAL_ROTATION = -20.0f;

// Reliquia - Constantes
constexpr float RELIQUIA_ROTATION_SPEED = 1.5f;  // Velocidad media de rotación
constexpr float RELIQUIA_BOUNCE_SPEED = 3.0f;    // Velocidad de saltitos (más rápida)
constexpr float RELIQUIA_BOUNCE_HEIGHT = 15.0f;  // Altura máxima del saltito (más notorio)

// Tridente - Constantes de animación de vuelo
constexpr float TRIDENTE_FLIGHT_SPEED = 0.02f;  // Velocidad del vuelo (0.0 a 1.0 por frame)


// VARIABLES DE ESTADO DE ANIMACIÓN



// Variables de posición y animación de Michelle
float michelleMovX = 0.0f;
float michelleMovZ = 0.0f;
float michelleRotation = MICHELLE_INITIAL_ROTATION;
float michelleFinalCounter = 0.0f;
MichelleState michelleState = WALKING_RIGHT;
bool animateMichelle = true;

// Variables de animación del perro
float perroPataDerDelantera = 0.0f;
float perroPataDerTrasera = 0.0f;
float perroPataIzqDelantera = 0.0f;
float perroPataIzqTrasera = 0.0f;
float perroCola = 0.0f;
float perroMovX = 0.0f;
bool perroAnimDirection = true;
bool perroMovDirection = true;
float perroRotacion = PERRO_INITIAL_ROTATION;
bool perroGirando = false;
float perroRotacionObjetivo = PERRO_INITIAL_ROTATION;

// Variables de animación de la reliquia
float reliquiaRotation = 0.0f;
float reliquiaBounceAngle = 0.0f;  // Ángulo para controlar el movimiento sinusoidal del salto

// Variables de control del tridente (escultura)
bool tridenteEnPosicionInicial = true;     // true = lejos, false = con la escultura
bool tridenteVolando = false;              // true cuando está animándose
float tridenteFlightProgress = 0.0f;       // 0.0 = inicio, 1.0 = destino
glm::vec3 tridentePosicionActual = glm::vec3(-660.0f, 145.0f, -730.0f);  // Posición actual interpolada
float tridenteRotacionActual = 0.0f;       // Rotación actual durante el vuelo
float tridenteLightFlicker = 0.0f;         // Tiempo tintineo luz

struct DroneAnimation {
	float currentAngle;
	float patternRotation;
	float trajectoryRadius;
	float verticalAmplitude;
	float baseHeight;
	float centerOffsetX;
	float centerOffsetZ;
	float scale;
	
	DroneAnimation() : 
		currentAngle(0.0f),
		patternRotation(0.0f),
		trajectoryRadius(800.0f),
		verticalAmplitude(500.0f),
		baseHeight(1250.0f),
		centerOffsetX(-3500.0f),
		centerOffsetZ(-2500.0f),
		scale(15.0f) {}
};

DroneAnimation droneAnim;

struct BustoKeyframe {
	float posX, posY, posZ;
	float rotX, rotZ;
	int durationSteps;
};

struct BustoAnimation {
	static const int MAX_KEYFRAMES = 12;
	BustoKeyframe keyframes[MAX_KEYFRAMES];
	int totalKeyframes;
	int currentKeyframe;
	int interpolationSteps;
	int currentStep;
	bool isPlaying;
	bool isPaused;
	
	float currentPosX, currentPosY, currentPosZ;
	float currentRotX, currentRotZ;
	float incrementPosX, incrementPosY, incrementPosZ;
	float incrementRotX, incrementRotZ;
	
	BustoAnimation() : 
		totalKeyframes(0), 
		currentKeyframe(0),
		interpolationSteps(90),
		currentStep(0),
		isPlaying(false),
		isPaused(false),
		currentPosX(0), currentPosY(10.0f), currentPosZ(0),
		currentRotX(0), currentRotZ(0),
		incrementPosX(0), incrementPosY(0), incrementPosZ(0),
		incrementRotX(0), incrementRotZ(0) {}
};

BustoAnimation bustoAnim;
// Sistema de Audio
struct AudioSystem {
	MIX_Mixer* mixer;
	MIX_Audio* museoMusic;     // Música del museo (interior)
	MIX_Audio* ciudadSound;    // Sonido de ciudad (exterior)
	MIX_Track* track;
	bool initialized;
	float volume;
	
	AudioSystem() : mixer(nullptr), museoMusic(nullptr), ciudadSound(nullptr), 
	                track(nullptr), initialized(false), volume(0.5f) {}
};

AudioSystem audioSystem;

// Rutas de archivos de audio
const char* museoMusicFile = "resources/audio/museo_music.mp3";
const char* ciudadSoundFile = "resources/audio/sonido_ciudad.mp3";

// InitAudio
void InitAudio() {
	std::cout << "=== Inicializando Sistema de Audio ===" << std::endl;
	
	//Inicializar SDL_mixer
	if (!MIX_Init()) {
		std::cerr << "ERROR: No se pudo inicializar SDL_mixer" << std::endl;
		std::cerr << "Error: " << SDL_GetError() << std::endl;
		return;
	}
	std::cout << "[OK] SDL_mixer inicializado" << std::endl;
	
	SDL_AudioSpec spec;
	spec.format = SDL_AUDIO_S16;
	spec.channels = 2;
	spec.freq = 44100;

	Uint32 initialized = SDL_WasInit(SDL_INIT_AUDIO);
	if (!(initialized & SDL_INIT_AUDIO)) {
		std::cerr << "ERROR: SDL (audio) no está inicializado" << std::endl;
		std::cerr << "       SDL debería haberse inicializado al inicio del programa" << std::endl;
		std::cerr << "\n[INFO] El programa continuará sin audio" << std::endl;
		MIX_Quit();
		return;
	}
	std::cout << "[OK] SDL (audio) está inicializado" << std::endl;
	

	const SDL_AudioDeviceID DEFAULT_OUTPUT = (SDL_AudioDeviceID)0xFFFFFFFF;
	
	std::cout << "[INFO] Verificando acceso a dispositivo de audio..." << std::endl;
	SDL_ClearError();
	SDL_AudioDeviceID testDevice = SDL_OpenAudioDevice(DEFAULT_OUTPUT, &spec);
	if (testDevice == 0) {
		const char* errorMsg = SDL_GetError();
		std::cerr << "ERROR: No se pudo abrir dispositivo de audio predeterminado" << std::endl;
		if (errorMsg && strlen(errorMsg) > 0) {
			std::cerr << "       Error: " << errorMsg << std::endl;
		}
		std::cerr << "\n[INFO] El programa continuará sin audio" << std::endl;
		MIX_Quit();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return;
	}
	std::cout << "[OK] Dispositivo de audio abierto correctamente (ID: " << testDevice << ")" << std::endl;
	SDL_CloseAudioDevice(testDevice);
	
	std::cout << "[INFO] Creando mixer de audio..." << std::endl;
	SDL_ClearError();
	audioSystem.mixer = MIX_CreateMixerDevice(DEFAULT_OUTPUT, &spec);
	if (!audioSystem.mixer) {
		const char* errorMsg = SDL_GetError();
		std::cerr << "ERROR: No se pudo crear el mixer de audio" << std::endl;
		if (errorMsg && strlen(errorMsg) > 0) {
			std::cerr << "       Error: " << errorMsg << std::endl;
		}
		std::cerr << "\n[INFO] El programa continuará sin audio" << std::endl;
		MIX_Quit();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return;
	}
	std::cout << "[OK] Mixer de audio creado" << std::endl;
	
	// Cargar música del museo
	audioSystem.museoMusic = MIX_LoadAudio(audioSystem.mixer, museoMusicFile, false);
	if (!audioSystem.museoMusic) {
		std::cout << "[WARN] No se pudo cargar la música del museo: " << museoMusicFile << std::endl;
		std::cout << "       Error: " << SDL_GetError() << std::endl;
	} else {
		std::cout << "[OK] Música del museo cargada: " << museoMusicFile << std::endl;
	}
	
	// Cargar sonido de ciudad
	audioSystem.ciudadSound = MIX_LoadAudio(audioSystem.mixer, ciudadSoundFile, false);
	if (!audioSystem.ciudadSound) {
		std::cout << "[WARN] No se pudo cargar el sonido de ciudad: " << ciudadSoundFile << std::endl;
		std::cout << "       Error: " << SDL_GetError() << std::endl;
	} else {
		std::cout << "[OK] Sonido de ciudad cargado: " << ciudadSoundFile << std::endl;
	}
	
	// Verificar que al menos uno se haya cargado
	if (!audioSystem.museoMusic && !audioSystem.ciudadSound) {
		std::cerr << "\n[ERROR] No se pudieron cargar los archivos de audio" << std::endl;
		std::cerr << "Verifique que existan los archivos MP3 en resources/audio/" << std::endl;
		MIX_DestroyMixer(audioSystem.mixer);
		audioSystem.mixer = nullptr;
		MIX_Quit();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return;
	}
	
	audioSystem.track = MIX_CreateTrack(audioSystem.mixer);
	if (!audioSystem.track) {
		std::cerr << "ERROR: No se pudo crear el track de audio" << std::endl;
		MIX_DestroyMixer(audioSystem.mixer);
		audioSystem.mixer = nullptr;
		MIX_Quit();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return;
	}
	
	MIX_SetTrackGain(audioSystem.track, audioSystem.volume);
	std::cout << "[OK] Volumen configurado: " << (int)(audioSystem.volume * 100) << "%" << std::endl;
	
	audioSystem.initialized = true;
	std::cout << "\n[OK] Sistema de audio listo" << std::endl;
	std::cout << "=====================================" << std::endl;
}
// PlayAudio
static bool PlayAudio(MIX_Audio* audio, const char* audioName, int loops = -1) {
	if (!audioSystem.initialized) {
		std::cout << "[AUDIO] Sistema de audio no inicializado" << std::endl;
		return false;
	}
	
	if (!audio || !audioSystem.track) {
		std::cout << "[AUDIO] " << audioName << " no disponible" << std::endl;
		return false;
	}
	
	if (MIX_TrackPlaying(audioSystem.track)) {
		MIX_StopTrack(audioSystem.track, 0);
	}
	
	MIX_SetTrackAudio(audioSystem.track, audio);
	
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, "SDL_mixer.loop.count", 
	                      loops == -1 ? MIX_DURATION_INFINITE : loops);
	
	bool success = MIX_PlayTrack(audioSystem.track, props);
	SDL_DestroyProperties(props);
	
	if (!success) {
		std::cerr << "[AUDIO] Error al reproducir " << audioName << std::endl;
		std::cerr << "        Error: " << SDL_GetError() << std::endl;
		return false;
	}
	
	std::cout << "[AUDIO] Reproduciendo " << audioName 
	          << (loops == -1 ? " (loop infinito)" : "") << std::endl;
	return true;
}

// PlayMuseoMusic
void PlayMuseoMusic(int loops = -1) {
	PlayAudio(audioSystem.museoMusic, "música del museo", loops);
}

// PlayCiudadSound
void PlayCiudadSound(int loops = -1) {
	PlayAudio(audioSystem.ciudadSound, "sonido de ciudad", loops);
}

// StopAudio
void StopAudio() {
	if (!audioSystem.initialized || !audioSystem.track) {
		return;
	}
	
	if (MIX_TrackPlaying(audioSystem.track)) {
		MIX_StopTrack(audioSystem.track, 0);  // 0 = sin fade out
		std::cout << "[AUDIO] Reproducción detenida" << std::endl;
	}
}

void CleanupAudio() {
	if (!audioSystem.initialized) {
		return;
	}
	
	std::cout << "\n=== Limpiando Sistema de Audio ===" << std::endl;
	
	if (audioSystem.track && MIX_TrackPlaying(audioSystem.track)) {
		MIX_StopTrack(audioSystem.track, 0);  // 0 = sin fade out
		std::cout << "[OK] Reproducción detenida" << std::endl;
	}
	
	if (audioSystem.track) {
		audioSystem.track = nullptr;
	}
	
	if (audioSystem.museoMusic) {
		audioSystem.museoMusic = nullptr;
		std::cout << "[OK] Música del museo liberada" << std::endl;
	}
	
	if (audioSystem.ciudadSound) {
		audioSystem.ciudadSound = nullptr;
		std::cout << "[OK] Sonido de ciudad liberado" << std::endl;
	}
	
	if (audioSystem.mixer) {
		MIX_DestroyMixer(audioSystem.mixer);
		audioSystem.mixer = nullptr;
		std::cout << "[OK] Mixer destruido" << std::endl;
	}
	
	MIX_Quit();
	std::cout << "[OK] SDL_mixer finalizado" << std::endl;
	
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	std::cout << "[OK] SDL (audio) finalizado" << std::endl;
	
	audioSystem.initialized = false;
	std::cout << "===================================\n" << std::endl;
}


// Funciones auxiliares para simplificar la animación de Michelle
inline bool updateMichelleRotation(float targetAngle, MichelleState nextState, bool clockwise) {
	if (clockwise) {
		michelleRotation += MICHELLE_ROTATION_SPEED;
		if (michelleRotation >= targetAngle) {
			michelleRotation = targetAngle;
			michelleState = nextState;
			return true;
		}
	} else {
		michelleRotation -= MICHELLE_ROTATION_SPEED;
		if (michelleRotation <= targetAngle) {
			michelleRotation = targetAngle;
			michelleState = nextState;
			return true;
		}
	}
	return false;
}

inline bool updateMichelleMovement(float& position, float speed, float target, MichelleState nextState) {
	position += speed;
	if ((speed > 0 && position >= target) || (speed < 0 && position <= target)) {
		position = target;
		michelleState = nextState;
		return true;
	}
	return false;
}

void animate(void) 
{
	// Animación de Michelle con estados
	if (animateMichelle)
	{
		switch (michelleState)
		{
		case WALKING_RIGHT:
			updateMichelleMovement(michelleMovX, MICHELLE_SPEED, MICHELLE_RANGE_MAX, TURNING_1);
			break;
			
		case TURNING_1:
			if (updateMichelleRotation(160.0f, WALKING_FORWARD, true))
				michelleMovZ = 0.0f;
			break;
			
		case WALKING_FORWARD:
			updateMichelleMovement(michelleMovZ, -MICHELLE_SPEED, -MICHELLE_WALK_DISTANCE, TURNING_2);
			break;
			
		case TURNING_2:
			if (updateMichelleRotation(250.0f, WALKING_FINAL, true))
				michelleFinalCounter = 0.0f;
			break;
		
		case WALKING_FINAL:
		case WALKING_BACK_1:
			// Simplificado: movimiento unificado para ambos casos
			michelleMovX += (michelleState == WALKING_FINAL) ? -MICHELLE_SPEED : MICHELLE_SPEED;
			michelleFinalCounter += MICHELLE_SPEED;
			if (michelleFinalCounter >= MICHELLE_FINAL_WALK) {
				michelleState = (michelleState == WALKING_FINAL) ? TURNING_180 : TURNING_BACK_1;
			}
			break;
		
		case TURNING_180:
			if (updateMichelleRotation(430.0f, WALKING_BACK_1, true)) {
				michelleRotation = MICHELLE_INITIAL_ROTATION;
				michelleFinalCounter = 0.0f;
			}
			break;
		
		case TURNING_BACK_1:
			if (updateMichelleRotation(-20.0f, WALKING_BACK_2, false))
				michelleRotation = 340.0f;
			break;
		
		case WALKING_BACK_2:
			updateMichelleMovement(michelleMovZ, MICHELLE_SPEED, 0.0f, TURNING_BACK_2);
			break;
		
		case TURNING_BACK_2:
			updateMichelleRotation(250.0f, WALKING_LEFT, false);
			break;
		
		case WALKING_LEFT:
			updateMichelleMovement(michelleMovX, -MICHELLE_SPEED, 0.0f, TURNING_FINAL);
			break;
		
		case TURNING_FINAL:
			if (updateMichelleRotation(430.0f, WALKING_RIGHT, true))
				michelleRotation = MICHELLE_INITIAL_ROTATION;
			break;
		}
	}

	updateDroneAnimation(droneAnim);
	updateBustoAnimation(bustoAnim);
	
	// Animación del perro
	if (perroAnimDirection) {
		perroPataDerDelantera += PERRO_ANIM_SPEED;
		perroPataDerTrasera -= PERRO_ANIM_SPEED;
		perroPataIzqDelantera -= PERRO_ANIM_SPEED;
		perroPataIzqTrasera += PERRO_ANIM_SPEED;
		perroCola += PERRO_ANIM_SPEED;
		
		if (perroPataDerDelantera >= PERRO_PATA_MAX) {
			perroAnimDirection = false;
		}
	} else {
		perroPataDerDelantera -= PERRO_ANIM_SPEED;
		perroPataDerTrasera += PERRO_ANIM_SPEED;
		perroPataIzqDelantera += PERRO_ANIM_SPEED;
		perroPataIzqTrasera -= PERRO_ANIM_SPEED;
		perroCola -= PERRO_ANIM_SPEED;
		
		if (perroPataDerDelantera <= -PERRO_PATA_MAX) {
			perroAnimDirection = true;
		}
	}
	
	// Movimiento de lado a lado
	constexpr float PERRO_VELOCIDAD_GIRO = 3.0f;
	if (perroGirando) {
		// Girar hacia el objetivo
		float diff = perroRotacionObjetivo - perroRotacion;
		if (std::abs(diff) <= PERRO_VELOCIDAD_GIRO) {
			perroRotacion = perroRotacionObjetivo;
			perroGirando = false;
		} else {
			perroRotacion += (diff > 0) ? PERRO_VELOCIDAD_GIRO : -PERRO_VELOCIDAD_GIRO;
		}
	} else {
		// Movimiento unificado
		float speed = perroMovDirection ? -PERRO_MOV_SPEED : PERRO_MOV_SPEED;
		float limit = perroMovDirection ? -PERRO_MOV_RANGE : 0.0f;
		
		perroMovX += speed;
		if ((perroMovDirection && perroMovX <= limit) || (!perroMovDirection && perroMovX >= limit)) {
			perroMovX = limit;
			perroGirando = true;
			perroRotacionObjetivo = perroRotacion + 180.0f;
			perroMovDirection = !perroMovDirection;
		}
	}
	
	// Animación de la reliquia
	// Rotación continua sobre su propio eje (eje Y)
	reliquiaRotation += RELIQUIA_ROTATION_SPEED;
	if (reliquiaRotation >= 360.0f) {
		reliquiaRotation -= 360.0f;
	}
	
	// Saltitos pequeños usando movimiento sinusoidal
	reliquiaBounceAngle += RELIQUIA_BOUNCE_SPEED;
	if (reliquiaBounceAngle >= 360.0f) {
		reliquiaBounceAngle -= 360.0f;
	}
	
	// Animación de vuelo del tridente
	if (tridenteVolando) {
		// Incrementar progreso del vuelo
		tridenteFlightProgress += TRIDENTE_FLIGHT_SPEED;
		
		// Posiciones de inicio y destino
		glm::vec3 posicionLejos = glm::vec3(-660.0f, 145.0f, -730.0f);
		glm::vec3 posicionCerca = glm::vec3(-700.0f, 150.0f, -360.0f);
		
		// Determinar dirección del vuelo
		glm::vec3 posInicio, posFin;
		if (tridenteEnPosicionInicial) {
			// Volando de cerca a lejos
			posInicio = posicionCerca;
			posFin = posicionLejos;
		} else {
			// Volando de lejos a cerca
			posInicio = posicionLejos;
			posFin = posicionCerca;
		}
		
		// Interpolación lineal de la posición (lerp)
		float t = tridenteFlightProgress;
		tridentePosicionActual = posInicio + (posFin - posInicio) * t;
		
		// Rotación durante el vuelo (90° en la mitad del trayecto)
		// Usar una curva sinusoidal para hacer la rotación más suave
		tridenteRotacionActual = sin(t * 3.14159f) * 90.0f;
		
		// Verificar si llegó al destino
		if (tridenteFlightProgress >= 1.0f) {
			tridenteVolando = false;
			tridenteFlightProgress = 1.0f;
			tridentePosicionActual = posFin;
			tridenteRotacionActual = 0.0f;
			std::cout << "[TRIDENTE] ¡Llegó a su destino!" << std::endl;
		}
	}
	
	// Animación de tintineo de la luz del tridente
	tridenteLightFlicker += 0.5f;  // Velocidad del tintineo
	if (tridenteLightFlicker >= 360.0f) {
		tridenteLightFlicker -= 360.0f;
	}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main() {
	SDL_SetHint(SDL_HINT_AUDIO_CATEGORY, "playback");
	
	std::cout << "\n[DIAGNÓSTICO] Drivers de audio disponibles:" << std::endl;
	int numDrivers = SDL_GetNumAudioDrivers();
	if (numDrivers > 0) {
		for (int i = 0; i < numDrivers; i++) {
			const char* driverName = SDL_GetAudioDriver(i);
			std::cout << "  [" << i << "] " << (driverName ? driverName : "NULL") << std::endl;
		}
	} else {
		std::cout << "  (No hay drivers disponibles)" << std::endl;
	}
	
	SDL_ClearError();
	int initResult = SDL_Init(SDL_INIT_AUDIO);
	if (initResult != 0) {
		const char* errorMsg = SDL_GetError();
		std::cerr << "\n╔══════════════════════════════════════════════════════════════╗" << std::endl;
		std::cerr << "║  ERROR: No se pudo inicializar SDL (audio)                  ║" << std::endl;
		std::cerr << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
		std::cerr << "Código de error: " << initResult << std::endl;
		if (errorMsg && strlen(errorMsg) > 0) {
			std::cerr << "Mensaje: " << errorMsg << std::endl;
		}
		
		const char* currentDriver = SDL_GetCurrentAudioDriver();
		if (currentDriver) {
			std::cerr << "Driver actual: " << currentDriver << std::endl;
		} else {
			std::cerr << "Driver actual: (ninguno)" << std::endl;
		}
		
		std::cerr << "\n📋 SOLUCIÓN - Verificar permisos de audio en macOS:" << std::endl;
		std::cerr << "   1. Abre: Preferencias del Sistema > Privacidad y Seguridad" << std::endl;
		std::cerr << "   2. Busca 'Grabación de pantalla y audio del sistema'" << std::endl;
		std::cerr << "   3. Asegúrate de que Terminal tenga permisos habilitados" << std::endl;
		std::cerr << "\n⚠️  El programa continuará pero el audio NO funcionará.\n" << std::endl;
	} else {
		std::cout << "[OK] SDL (audio) inicializado correctamente" << std::endl;
		const char* currentDriver = SDL_GetCurrentAudioDriver();
		if (currentDriver) {
			std::cout << "[INFO] Driver de audio activo: " << currentDriver << std::endl;
		}
	}
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pratica X 2026-1", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	InitAudio();
	glEnable(GL_DEPTH_TEST);

	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");
	
	vector<std::string> faces{
		"resources/skybox/right.png",
		"resources/skybox/left.png",
		"resources/skybox/top.png",
		"resources/skybox/bottom.png",
		"resources/skybox/front.png",
		"resources/skybox/back.png"
	};
	Skybox skybox = Skybox(faces);

	Model escenario("resources/objects/Escenario/museoFinal.obj");
	Model drone("resources/objects/Drone/drone.obj");
	Model busto("resources/objects/Busto/busto.obj");
	Model david("resources/objects/David/david.obj");
	Model reliquia("resources/objects/Reliquia/reliquia.obj");
	Model totem("resources/objects/Totem/totem.obj");
	Model estatua("resources/objects/Estatua/old_man.obj");
	
	// Shiva con tridente
	Model esculturaBase("resources/objects/Escultura/escultura.obj");
	Model esculturaTridente("resources/objects/Escultura/tridente.obj");
	Model esculturaAdorno("resources/objects/Escultura/adorno.obj");
	
	// Perro - todas las partes
	Model perroCuerpo("resources/objects/Perro/cuerpo.obj");
	Model perroCola("resources/objects/Perro/cola.obj");
	Model perroPataDerDel("resources/objects/Perro/pataD_Right.obj");
	Model perroPataIzqDel("resources/objects/Perro/pataD_left.obj");
	Model perroPataDerTra("resources/objects/Perro/pataT_right.obj");
	Model perroPataIzqTra("resources/objects/Perro/pataT_left.obj");

	ModelAnim animacionPersonaje("resources/objects/Joe/joe.dae");
	animacionPersonaje.initShaders(animShader.ID);

	ModelAnim caminaMichelle("resources/objects/Michelle/michelle.dae");
	caminaMichelle.initShaders(animShader.ID);

	initBustoKeyframes(bustoAnim);

	glm::mat4 modelOp = glm::mat4(1.0f);
	glm::mat4 viewOp = glm::mat4(1.0f);
	glm::mat4 projectionOp = glm::mat4(1.0f);

	PlayMuseoMusic(-1);

	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		processInput(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", glm::vec3(0.0f, -1.0f, -1.0f));
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		// Luz principal (general)
		setupPointLight(staticShader, 0, 
			glm::vec3(70.0f, 25.0f, 0.0f),
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			0.08f, 0.009f, 0.00032f);

		// Luz tipo sol para zona de esculturas (reliquia, totem, estatua, escultura)
		setupPointLight(staticShader, 1,
			glm::vec3(-820.0f, 300.0f, -600.0f),
			glm::vec3(0.3f, 0.3f, 0.3f),
			glm::vec3(1.2f, 1.2f, 1.0f),
			glm::vec3(1.0f, 1.0f, 0.9f),
			1.0f, 0.003f, 0.00008f);

		// Luz dorada tridente con tintineo
		float sinValue = sin(glm::radians(tridenteLightFlicker * 5.0f));
		float normalizedSin = (sinValue + 1.0f) * 0.5f;
		float flickerIntensity = 0.1f + 0.9f * normalizedSin;
		setupPointLight(staticShader, 2,
			glm::vec3(-660.0f, 180.0f, -730.0f),
			glm::vec3(0.6f * flickerIntensity, 0.5f * flickerIntensity, 0.15f * flickerIntensity),
			glm::vec3(4.0f * flickerIntensity, 3.2f * flickerIntensity, 0.8f * flickerIntensity),
			glm::vec3(5.0f * flickerIntensity, 4.0f * flickerIntensity, 1.5f * flickerIntensity),
			1.0f, 0.007f, 0.0002f);

		// Luz dorada suave para el adorno
		setupPointLight(staticShader, 3,
			glm::vec3(-690.0f, 260.0f, -358.0f),
			glm::vec3(0.25f, 0.2f, 0.08f),
			glm::vec3(1.2f, 1.0f, 0.3f),
			glm::vec3(1.4f, 1.2f, 0.5f),
			1.0f, 0.009f, 0.0003f);

		// Linterna de la cámara (spotLight)
		setupSpotLight(staticShader, 0,
			camera.Position, camera.Front,
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::cos(glm::radians(10.0f)), glm::cos(glm::radians(15.0f)),
			1.0f, 0.0014f, 0.000007f);

		staticShader.setFloat("material_shininess", 32.0f);

		// View/projection transformations
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		// Personajes animados
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.1f));
		animShader.setFloat("material.shininess", 8.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		// Joe
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));	
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);

		// Michelle
		float michelleBaseX = -1100.0f;
		float michelleY = 0.0f;
		float michelleZ = 50.0f;
		float michelleScale = 2.0f;

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(michelleBaseX + michelleMovX, michelleY, michelleZ + michelleMovZ));
		modelOp = glm::rotate(modelOp, glm::radians(michelleRotation), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(michelleScale));
		animShader.setMat4("model", modelOp);
		caminaMichelle.Draw(animShader);

		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		// Museo
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(60.0f)); 
		staticShader.setMat4("model", modelOp);
		escenario.Draw(staticShader);

		renderDrone(drone, staticShader, droneAnim);
		renderBusto(busto, staticShader, bustoAnim);
		renderdavid(david, staticShader);
		renderReliquia(reliquia, staticShader);
		renderTotem(totem, staticShader);
		renderEstatua(estatua, staticShader);
		renderEscultura(esculturaBase, esculturaTridente, esculturaAdorno, staticShader);
		renderPerro(perroCuerpo, perroCola, perroPataDerDel, perroPataIzqDel, 
				    perroPataDerTra, perroPataIzqTra, staticShader);

		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	skybox.Terminate();
	CleanupAudio();
	glfwTerminate();
	return 0;
}

void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// ========================================================================
	// SISTEMA DE AUDIO Y CÁMARA - Control por teclado
	// ========================================================================
	// Tecla E (Exterior) - Vista exterior del museo y reproducir sonido de ciudad
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		camera.Position = glm::vec3(-3000.0f, 0.0f, -1000.0f);
		camera.Front = glm::normalize(glm::vec3(3000.0f, 0.0f, 1000.0f));
		camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
		
		PlayCiudadSound(-1);  // Loop infinito
		std::cout << "[CAMARA] Vista exterior del museo - Sonido de ciudad" << std::endl;
	}

	// Tecla I (Interior) - Vista interior y reproducir música del museo
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		camera.Position = glm::vec3(0.0f, 200.0f, 800.0f);
		camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
		camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
		
		PlayMuseoMusic(-1);  // Loop infinito
		std::cout << "[CAMARA] Vista interior del museo - Música del museo" << std::endl;
	}

	// Tecla Z (Zoom) - Vista de zoom a los modelos
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		camera.Position = glm::vec3(952.932f, 203.219f, 40.9843f);
		camera.Front = glm::normalize(glm::vec3(0.253213f, 0.046848f, -0.966276f));
		camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
		
		std::cout << "[CAMARA] Vista de zoom a modelos" << std::endl;
	}

	// Tecla R (Reliquia) - Vista de la reliquia
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		camera.Position = glm::vec3(-517.207f, 225.778f, -452.803f);
		camera.Front = glm::normalize(glm::vec3(-0.910688f, -0.0648589f, -0.407972f));
		camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
		
		std::cout << "[CAMARA] Vista de la reliquia" << std::endl;
	}

	// Tecla P - Imprimir posición actual de la cámara
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		std::cout << "\n========================================" << std::endl;
		std::cout << "[CAMARA] Posicion actual:" << std::endl;
		std::cout << "  Position: (" << camera.Position.x << ", " 
			      << camera.Position.y << ", " << camera.Position.z << ")" << std::endl;
		std::cout << "  Front:    (" << camera.Front.x << ", " 
			      << camera.Front.y << ", " << camera.Front.z << ")" << std::endl;
		std::cout << "  Yaw:      " << camera.Yaw << " grados" << std::endl;
		std::cout << "  Pitch:    " << camera.Pitch << " grados" << std::endl;
		std::cout << "========================================\n" << std::endl;
	}

	// Tecla 0 - Tridente volador
	if (key == GLFW_KEY_0 && action == GLFW_PRESS && !tridenteVolando)
	{
		// Iniciar animación de vuelo
		tridenteVolando = true;
		tridenteFlightProgress = 0.0f;
		tridenteEnPosicionInicial = !tridenteEnPosicionInicial;
		
		if (tridenteEnPosicionInicial) {
			std::cout << "[TRIDENTE] ¡Volando de regreso a posición lejana!" << std::endl;
		} else {
			std::cout << "[TRIDENTE] ¡Volando hacia la escultura!" << std::endl;
		}
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		playBustoAnimation(bustoAnim);
		std::cout << "[BUSTO] Animacion iniciada" << std::endl;
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		pauseBustoAnimation(bustoAnim);
		std::cout << "[BUSTO] Animacion pausada" << std::endl;
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		resetBustoAnimation(bustoAnim);
		std::cout << "[BUSTO] Animacion reseteada" << std::endl;
	}


}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void updateDroneAnimation(DroneAnimation& anim) {
	anim.currentAngle += DroneAnimConfig::ANGLE_INCREMENT;
	anim.patternRotation += DroneAnimConfig::ROTATION_INCREMENT;
}

glm::vec3 calculateDronePosition(const DroneAnimation& anim) {
	float localX = anim.trajectoryRadius * sin(anim.currentAngle);
	float localZ = anim.trajectoryRadius * sin(2.0f * anim.currentAngle);
	float height = anim.baseHeight + anim.verticalAmplitude * sin(anim.currentAngle * 0.5f);
	
	float rotatedX = localX * cos(anim.patternRotation) - localZ * sin(anim.patternRotation);
	float rotatedZ = localX * sin(anim.patternRotation) + localZ * cos(anim.patternRotation);
	
	float finalX = rotatedX + anim.centerOffsetX;
	float finalZ = rotatedZ + anim.centerOffsetZ;
	
	return glm::vec3(finalX, height, finalZ);
}


// Funciones de renderizado del Drone

void renderDrone(Model& droneModel, Shader& shader, const DroneAnimation& anim) {
	glm::vec3 position = calculateDronePosition(anim);
	
	glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
	model = glm::rotate(model, anim.currentAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(anim.scale));
	
	shader.setMat4("model", model);
	droneModel.Draw(shader);
}

// Funcin para renderizar una pata del perro
inline void renderPataPerro(const glm::mat4& base, float rotacion, float scale, Model& pata, Shader& shader) {
	glm::mat4 model = glm::rotate(base, glm::radians(rotacion), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(scale));
	shader.setMat4("model", model);
	pata.Draw(shader);
}

void renderPerro(Model& cuerpo, Model& cola, Model& pataDerDel, Model& pataIzqDel, 
			     Model& pataDerTra, Model& pataIzqTra, Shader& shader) {
	constexpr float perroBaseX = -5000.0f;
	constexpr float perroBaseY = -1200.0f;
	constexpr float perroBaseZ = -5000.0f;
	constexpr float perroScale = 50.0f;
	
	glm::mat4 modelBase = glm::translate(glm::mat4(1.0f), glm::vec3(perroBaseX + perroMovX, perroBaseY, perroBaseZ));
	modelBase = glm::rotate(modelBase, glm::radians(perroRotacion), glm::vec3(0.0f, 1.0f, 0.0f));
	
	renderPataPerro(modelBase, perroPataDerDelantera, perroScale, pataDerDel, shader);
	renderPataPerro(modelBase, perroPataIzqDelantera, perroScale, pataIzqDel, shader);
	renderPataPerro(modelBase, perroPataDerTrasera, perroScale, pataDerTra, shader);
	renderPataPerro(modelBase, perroPataIzqTrasera, perroScale, pataIzqTra, shader);
	
	// Cola
	glm::mat4 modelCola = glm::rotate(modelBase, glm::radians(perroCola), glm::vec3(0.0f, 1.0f, 0.0f));
	modelCola = glm::scale(modelCola, glm::vec3(perroScale));
	shader.setMat4("model", modelCola);
	cola.Draw(shader);
	
	// Cuerpo
	modelBase = glm::scale(modelBase, glm::vec3(perroScale));
	shader.setMat4("model", modelBase);
	cuerpo.Draw(shader);
}

void initBustoKeyframes(BustoAnimation& anim) {
	using namespace BustoAnimConfig;
	
	anim.keyframes[0]  = {BASE_X, INITIAL_HEIGHT,              BASE_Z,           0.0f,   0.0f,  90};
	anim.keyframes[1]  = {BASE_X, INITIAL_HEIGHT,              BASE_Z,           5.0f,   2.0f,  80};
	anim.keyframes[2]  = {BASE_X, INITIAL_HEIGHT,              BASE_Z,          -5.0f,  -2.0f,  80};
	anim.keyframes[3]  = {BASE_X, INITIAL_HEIGHT,              BASE_Z,          15.0f,   5.0f,  70};
	anim.keyframes[4]  = {BASE_X, INITIAL_HEIGHT,              BASE_Z,          30.0f,  10.0f,  60};
	anim.keyframes[5]  = {BASE_X + 1.0f, INITIAL_HEIGHT * 0.5f, BASE_Z + 1.0f,  60.0f,  30.0f,  50};
	anim.keyframes[6]  = {BASE_X + 2.0f, 60.0f,                BASE_Z + 2.0f,   90.0f,  90.0f,  40};
	anim.keyframes[7]  = {BASE_X + 3.0f, 45.0f,                BASE_Z + 3.0f,   95.0f,  95.0f,  30};
	anim.keyframes[8]  = {BASE_X + 4.0f, FINAL_HEIGHT,         BASE_Z + 4.0f,   90.0f,  90.0f,  20};
	anim.keyframes[9]  = {BASE_X + 5.0f, FINAL_HEIGHT,         BASE_Z + 5.0f,   90.0f,  90.0f,  15};
	anim.keyframes[10] = {BASE_X + 6.0f, FINAL_HEIGHT,         BASE_Z + 6.0f,   90.0f,  90.0f,  15};
	
	anim.totalKeyframes = 11;
	anim.currentPosX = anim.keyframes[0].posX;
	anim.currentPosY = anim.keyframes[0].posY;
	anim.currentPosZ = anim.keyframes[0].posZ;
	anim.currentRotX = anim.keyframes[0].rotX;
	anim.currentRotZ = anim.keyframes[0].rotZ;
}

void calculateBustoInterpolation(BustoAnimation& anim) {
	const int curr = anim.currentKeyframe;
	const int next = curr + 1;
	const float steps = static_cast<float>(anim.keyframes[curr].durationSteps);
	
	auto calcIncrement = [steps](float nextVal, float currVal) {
		return (nextVal - currVal) / steps;
	};
	
	anim.incrementPosX = calcIncrement(anim.keyframes[next].posX, anim.keyframes[curr].posX);
	anim.incrementPosY = calcIncrement(anim.keyframes[next].posY, anim.keyframes[curr].posY);
	anim.incrementPosZ = calcIncrement(anim.keyframes[next].posZ, anim.keyframes[curr].posZ);
	anim.incrementRotX = calcIncrement(anim.keyframes[next].rotX, anim.keyframes[curr].rotX);
	anim.incrementRotZ = calcIncrement(anim.keyframes[next].rotZ, anim.keyframes[curr].rotZ);
	
	anim.interpolationSteps = anim.keyframes[curr].durationSteps;
}

void updateBustoAnimation(BustoAnimation& anim) {
	if (!anim.isPlaying || anim.isPaused) return;
	
	if (anim.currentStep >= anim.interpolationSteps) {
		anim.currentKeyframe++;
		
		if (anim.currentKeyframe >= anim.totalKeyframes - 1) {
			anim.isPlaying = false;
			anim.currentKeyframe = 0;
			anim.currentStep = 0;
			std::cout << "[BUSTO] Animacion completada" << std::endl;
			return;
		}
		
		anim.currentStep = 0;
		calculateBustoInterpolation(anim);
	}
	
	anim.currentPosX += anim.incrementPosX;
	anim.currentPosY += anim.incrementPosY;
	anim.currentPosZ += anim.incrementPosZ;
	anim.currentRotX += anim.incrementRotX;
	anim.currentRotZ += anim.incrementRotZ;
	
	anim.currentStep++;
}

void playBustoAnimation(BustoAnimation& anim) {
	if (!anim.isPlaying) {
		anim.currentKeyframe = 0;
		anim.currentStep = 0;
		anim.currentPosX = anim.keyframes[0].posX;
		anim.currentPosY = anim.keyframes[0].posY;
		anim.currentPosZ = anim.keyframes[0].posZ;
		anim.currentRotX = anim.keyframes[0].rotX;
		anim.currentRotZ = anim.keyframes[0].rotZ;
		calculateBustoInterpolation(anim);
	}
	anim.isPlaying = true;
	anim.isPaused = false;
}

void pauseBustoAnimation(BustoAnimation& anim) {
	anim.isPaused = !anim.isPaused;
}

void resetBustoAnimation(BustoAnimation& anim) {
	anim.isPlaying = false;
	anim.isPaused = false;
	anim.currentKeyframe = 0;
	anim.currentStep = 0;
	anim.currentPosX = anim.keyframes[0].posX;
	anim.currentPosY = anim.keyframes[0].posY;
	anim.currentPosZ = anim.keyframes[0].posZ;
	anim.currentRotX = anim.keyframes[0].rotX;
	anim.currentRotZ = anim.keyframes[0].rotZ;
}

void renderBusto(Model& bustoModel, Shader& shader, const BustoAnimation& anim) {
	using namespace BustoRenderConfig;
	
	glm::vec3 bustoPos(
		BASE_POS.x + PIVOT_OFFSET.x, 
		anim.currentPosY + PIVOT_OFFSET.y, 
		BASE_POS.z + PIVOT_OFFSET.z
	);
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, bustoPos);
	model = glm::rotate(model, glm::radians(anim.currentRotZ), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(anim.currentRotX), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(SCALE));
	shader.setMat4("model", model);
	bustoModel.Draw(shader);
}


// Funciones de renderizado de modelos estáticos del museo

void renderdavid(Model& davidModel, Shader& shader) {
	// Renderizar el modelo de David con transformaciones específicas
	// Posición: (995, 155, -36), Rotación: 90° en Y, Escala: 0.6
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(995.0f, 153.0f, -36.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.6f));
	shader.setMat4("model", model);
	davidModel.Draw(shader);
}

void renderReliquia(Model& reliquiaModel, Shader& shader) {
	glm::mat4 model = glm::mat4(1.0f);
	
	float bounceOffset = abs(sin(glm::radians(reliquiaBounceAngle))) * RELIQUIA_BOUNCE_HEIGHT;
	
	model = glm::translate(model, glm::vec3(-791.0f, 150.0f + bounceOffset, -567.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(reliquiaRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.5f));
	
	shader.setMat4("model", model);
	reliquiaModel.Draw(shader);
}

void renderTotem(Model& totemModel, Shader& shader) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1046.0f, 150.0f, -440.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f));
	shader.setMat4("model", model);
	totemModel.Draw(shader);
}

void renderEstatua(Model& estatuaModel, Shader& shader) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-740.0f, 150.0f, -1020.0f));
	model = glm::rotate(model, glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(3.0f));
	shader.setMat4("model", model);
	estatuaModel.Draw(shader);
}

void renderEscultura(Model& base, Model& tridente, Model& adorno, Shader& shader) {
	glm::vec3 esculturaPos = glm::vec3(-700.0f, 150.0f, -360.0f);
	float esculturaRotY = 60.0f;
	
	glm::mat4 modelBase = glm::mat4(1.0f);
	modelBase = glm::translate(modelBase, esculturaPos);
	modelBase = glm::rotate(modelBase, glm::radians(esculturaRotY), glm::vec3(0.0f, 1.0f, 0.0f));
	modelBase = glm::scale(modelBase, glm::vec3(2.0f));
	
	shader.setMat4("model", modelBase);
	base.Draw(shader);
	
	// Tridente
	glm::mat4 modelTridente;
	if (tridenteVolando) {
		modelTridente = glm::translate(glm::mat4(1.0f), tridentePosicionActual);
		modelTridente = glm::rotate(modelTridente, glm::radians(tridenteRotacionActual), glm::vec3(0.0f, 0.0f, 1.0f));
	} else if (tridenteEnPosicionInicial) {
		modelTridente = glm::translate(glm::mat4(1.0f), glm::vec3(-660.0f, 145.0f, -750.0f));
	} else {
		modelTridente = modelBase;
	}
	modelTridente = glm::scale(modelTridente, glm::vec3(2.0f));
	
	shader.setMat4("model", modelTridente);
	tridente.Draw(shader);
	
	glm::mat4 modelAdorno = modelBase;
	shader.setMat4("model", modelAdorno);
	adorno.Draw(shader);
}