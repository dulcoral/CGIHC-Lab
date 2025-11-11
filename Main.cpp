/*---------------------------------------------------------*/
/* ----------------   Proyecto Final --------------------------*/
/*-----------------    2026-1   ---------------------------*/
/*------------- Alumno: Dulce Coral Rodriguez Garcia    ---------------*/
/*------------- No. Cuenta 313144545     ---------------*/

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
#include <SDL3/SDL_audio.h>  // Para SDL_AUDIO_DEVICE_DEFAULT_OUTPUT
#include <SDL3_mixer/SDL_mixer.h>


#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
// STL
#include <vector>
#include <string>
#include <cstring>  // Para strlen
#include <cmath>     // Para cos, sin
// #include <mmsystem.h>



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

glm::vec3 calculateDronePosition(const DroneAnimation& anim);
void updateDroneAnimation(DroneAnimation& anim);
void renderDrone(Model& droneModel, Shader& shader, const DroneAnimation& anim);

void initBustoKeyframes(BustoAnimation& anim);
void updateBustoAnimation(BustoAnimation& anim);
void playBustoAnimation(BustoAnimation& anim);
void pauseBustoAnimation(BustoAnimation& anim);
void resetBustoAnimation(BustoAnimation& anim);
void renderBusto(Model& bustoModel, Shader& shader, const BustoAnimation& anim);

void renderPerro(Model& cuerpo, Model& cola, Model& pataDerDel, Model& pataIzqDel, 
                 Model& pataDerTra, Model& pataIzqTra, Shader& shader);

// GLFW error callback to diagnose initialization issues on macOS
static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

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
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la pr�ctica 6
unsigned int generateTextures(char*, bool, bool);	// De la pr�ctica 6

// Keyboard variables (no longer needed - camera controlled by WASD)

//Texture
unsigned int	t_smile,
t_toalla,
t_unam,
t_white,
t_ladrillos;

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

// Variables de posición y animación de Michelle
float michelleMovX = 0.0f;
float michelleMovZ = 0.0f;
float michelleRotation = 70.0f;
float michelleSpeed = 2.0f;
float michelleRangeMax = 650.0f;
float michelleWalkDistance = 200.0f;
float michelleFinalWalk = 100.0f;
float michelleFinalCounter = 0.0f;
float michelleRotationSpeed = 2.0f;
MichelleState michelleState = WALKING_RIGHT;
bool animateMichelle = true;

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

// ============================================================================
// Sistema de Audio - Variables Globales
// ============================================================================
struct AudioSystem {
    MIX_Mixer* mixer;
    MIX_Audio* backgroundMusic;
    MIX_Track* track;
    bool initialized;
    bool isPlaying;
    float volume;
    
    AudioSystem() : mixer(nullptr), backgroundMusic(nullptr), track(nullptr),
                    initialized(false), isPlaying(false), volume(0.5f) {}
};

AudioSystem audioSystem;

// Ruta archivo de audio
const char* audioFile = "resources/audio/sonido_ciudad.mp3";

//Keyframes (Manipulaci�n y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f;
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotRodIzqInc = 0.0f,
		giroMonitoInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir n�mero en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	
	if(isPrimitive)
		stbi_set_flip_vertically_on_load(true); 
	else
		stbi_set_flip_vertically_on_load(false);


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{

	t_smile = generateTextures("Texturas/awesomeface.png", 1, true);
	t_toalla = generateTextures("Texturas/toalla.tga", 0, true);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}

// ============================================================================
// InitAudio - Inicializa el sistema de audio SDL_mixer
// ============================================================================
void InitAudio() {
    std::cout << "=== Inicializando Sistema de Audio ===" << std::endl;
    
    //Inicializar SDL_mixer
    if (!MIX_Init()) {
        std::cerr << "ERROR: No se pudo inicializar SDL_mixer" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return;
    }
    std::cout << "[OK] SDL_mixer inicializado" << std::endl;
    
    // Configurar formato audio
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_S16;  // 16-bit signed
    spec.channels = 2;            // Estéreo
    spec.freq = 44100;            // 44.1kHz (calidad CD)
    

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
    
    audioSystem.backgroundMusic = MIX_LoadAudio(audioSystem.mixer, audioFile, false);
    
    if (!audioSystem.backgroundMusic) {
        std::cout << "[WARN] No se pudo cargar el audio: " << audioFile << std::endl;
        std::cout << "       Error: " << SDL_GetError() << std::endl;
        std::cerr << "\n[ERROR] No se cargó el audio" << std::endl;
        std::cerr << "Verifique que exista el archivo MP3 en resources/audio/" << std::endl;
        MIX_DestroyMixer(audioSystem.mixer);
        audioSystem.mixer = nullptr;
        MIX_Quit();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
    
    std::cout << "[OK] Audio cargado: " << audioFile << std::endl;
    
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

// ============================================================================
// PlayAudioTrack - Reproduce el audio de fondo
// Parámetros:
//   - loops: Número de repeticiones (-1 = infinito, 0 = una vez)
// ============================================================================
void PlayAudioTrack(int loops = -1) {
    if (!audioSystem.initialized) {
        std::cout << "[AUDIO] Sistema de audio no inicializado" << std::endl;
        return;
    }
    
    if (!audioSystem.backgroundMusic || !audioSystem.track) {
        std::cout << "[AUDIO] Audio no disponible" << std::endl;
        return;
    }
    
    if (MIX_TrackPlaying(audioSystem.track)) {
        MIX_StopTrack(audioSystem.track, 0);
    }
    
    MIX_SetTrackAudio(audioSystem.track, audioSystem.backgroundMusic);
    
    // Crear propiedades para la reproducción
    SDL_PropertiesID props = SDL_CreateProperties();
    if (loops == -1) {
        SDL_SetNumberProperty(props, "SDL_mixer.loop.count", MIX_DURATION_INFINITE);
    } else {
        SDL_SetNumberProperty(props, "SDL_mixer.loop.count", loops);
    }
    
    // Reproducir audio
    if (!MIX_PlayTrack(audioSystem.track, props)) {
        std::cerr << "[AUDIO] Error al reproducir audio" << std::endl;
        std::cerr << "        Error: " << SDL_GetError() << std::endl;
        SDL_DestroyProperties(props);  // Liberar propiedades en caso de error
        return;
    }

    
    audioSystem.isPlaying = true;
    
    std::cout << "[AUDIO] Reproduciendo audio de ambiente urbano" 
              << (loops == -1 ? " (loop infinito)" : "") << std::endl;
}

// Detiene la reproducción actual
void StopAudio() {
    if (!audioSystem.initialized || !audioSystem.track) {
        return;
    }
    
    if (MIX_TrackPlaying(audioSystem.track)) {
        MIX_StopTrack(audioSystem.track, 0);  // 0 = sin fade out
        audioSystem.isPlaying = false;
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
    
    if (audioSystem.backgroundMusic) {
        audioSystem.backgroundMusic = nullptr;
        std::cout << "[OK] Audio liberado" << std::endl;
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
		michelleRotation += michelleRotationSpeed;
		if (michelleRotation >= targetAngle) {
			michelleRotation = targetAngle;
			michelleState = nextState;
			return true;
		}
	} else {
		michelleRotation -= michelleRotationSpeed;
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
   /* lightPosition.x = 70.0f * cos(my_angle);
    lightPosition.z = 70.0f * sin(my_angle);

    my_angle += 0.01f;*/

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}

	// Animación de Michelle con estados
	if (animateMichelle)
	{
		switch (michelleState)
		{
		case WALKING_RIGHT:
			updateMichelleMovement(michelleMovX, michelleSpeed, michelleRangeMax, TURNING_1);
			break;
			
		case TURNING_1:
			if (updateMichelleRotation(160.0f, WALKING_FORWARD, true))
				michelleMovZ = 0.0f;
			break;
			
		case WALKING_FORWARD:
			updateMichelleMovement(michelleMovZ, -michelleSpeed, -michelleWalkDistance, TURNING_2);
			break;
			
		case TURNING_2:
			if (updateMichelleRotation(250.0f, WALKING_FINAL, true))
				michelleFinalCounter = 0.0f;
			break;
		
		case WALKING_FINAL:
			michelleMovX -= michelleSpeed;
			michelleFinalCounter += michelleSpeed;
			if (michelleFinalCounter >= michelleFinalWalk)
				michelleState = TURNING_180;
			break;
		
		case TURNING_180:
			if (updateMichelleRotation(430.0f, WALKING_BACK_1, true)) {
				michelleRotation = 70.0f;
				michelleFinalCounter = 0.0f;
			}
			break;
		
		case WALKING_BACK_1:
			michelleMovX += michelleSpeed;
			michelleFinalCounter += michelleSpeed;
			if (michelleFinalCounter >= michelleFinalWalk)
				michelleState = TURNING_BACK_1;
			break;
		
		case TURNING_BACK_1:
			if (updateMichelleRotation(-20.0f, WALKING_BACK_2, false))
				michelleRotation = 340.0f;
			break;
		
		case WALKING_BACK_2:
			updateMichelleMovement(michelleMovZ, michelleSpeed, 0.0f, TURNING_BACK_2);
			break;
		
		case TURNING_BACK_2:
			updateMichelleRotation(250.0f, WALKING_LEFT, false);
			break;
		
		case WALKING_LEFT:
			updateMichelleMovement(michelleMovX, -michelleSpeed, 0.0f, TURNING_FINAL);
			break;
		
		case TURNING_FINAL:
			if (updateMichelleRotation(430.0f, WALKING_RIGHT, true))
				michelleRotation = 70.0f;
			break;
		}
	}

	updateDroneAnimation(droneAnim);
	updateBustoAnimation(bustoAnim);
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	/*float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};*/

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	/*glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
    // Configurar hint de categoria de audio para macOS ANTES de inicializar SDL
    // Esto es necesario para que SDL3 funcione correctamente en macOS 15.5 (Sequoia)
    SDL_SetHint(SDL_HINT_AUDIO_CATEGORY, "playback");
    
    // Listar drivers de audio disponibles antes de inicializar
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
    
    // En macOS, SDL puede necesitar permisos de audio que se solicitan automaticamente
    SDL_ClearError();
    
    // Intentar inicializar SDL con audio
    // Nota: En macOS, la primera vez que se ejecuta puede aparecer un dialogo pidiendo permisos
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
        
        // Mostrar info del driver actual
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
        std::cerr << "\n⚠️  El programa continuará pero el audio NO funcionará." << std::endl;
        std::cerr << std::endl;
    } else {
        std::cout << "[OK] SDL (audio) inicializado correctamente" << std::endl;
        const char* currentDriver = SDL_GetCurrentAudioDriver();
        if (currentDriver) {
            std::cout << "[INFO] Driver de audio activo: " << currentDriver << std::endl;
        }
    }
    
    // glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif
	// glfw window creation
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

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	InitAudio();        // Inicializar sistema de audio
	myData();
	glEnable(GL_DEPTH_TEST);

	

	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 
	
	vector<std::string> faces{
		"resources/skybox/right.png",
		"resources/skybox/left.png",
		"resources/skybox/top.png",
		"resources/skybox/bottom.png",
		"resources/skybox/front.png",
		"resources/skybox/back.png"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------

	// load models
	// -----------
	//Model piso("resources/objects/piso/piso.obj");
	// Model carro("resources/objects/lambo/carroceria.obj");
	// Model llanta("resources/objects/lambo/Wheel.obj");
	// Model casaVieja("resources/objects/casa/OldHouse.obj");
	// //Model cubo("resources/objects/cubo/cube02.obj");
	// Model casaDoll("resources/objects/Casa/DollHouse.obj");
	// Model casaBruja("resources/objects/CasaBrujas/brujas.obj");
	// Model caja("resources/objects/Caja/cajaTextura.obj");
	// Model r2d2("resources/objects/R2D2/r2d2.obj");
	// Model aquaman("resources/objects/Aquaman/aquaman.obj");
	// Model aquaCuerpo("resources/objects/Aquaman/torso.obj");
	// Model aquaBrazoDerecho("resources/objects/Aquaman/brazoDer.obj");
	// Model aquaBrazoIzquierdo("resources/objects/Aquaman/brazoIzq.obj");
	// Model aquaPiernaDerecha("resources/objects/Aquaman/piernaDer.obj");
	// Model aquaPiernaIzquierda("resources/objects/Aquaman/piernaIzq.obj");
	// Model aquaCabeza("resources/objects/Aquaman/cabeza.obj");
	Model escenario("resources/objects/Escenario/museoFinal.obj");
	Model drone("resources/objects/Drone/drone.obj");
	Model busto("resources/objects/Busto/busto.obj");
	
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


	//Inicializaci�n de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}

	initBustoKeyframes(bustoAnim);

	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	// render loop
	// -----------
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

		staticShader.setVec3("pointLight[0].position", glm::vec3(70.0f, 25.0f, 0.0f));
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.00032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 20.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

        staticShader.setVec3("pointLight[2].position", glm::vec3(-80.0, 20.0f, 0.0f));
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 1.0f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(camera.Front.x, camera.Front.y, camera.Front.z));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(15.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0014f);
		staticShader.setFloat("spotLight[0].quadratic", 0.000007f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
	animShader.use();
	animShader.setMat4("projection", projectionOp);
	animShader.setMat4("view", viewOp);

	animShader.setVec3("material.specular", glm::vec3(0.1f));  // Reducido para menos brillo
	animShader.setFloat("material.shininess", 8.0f);           // Reducido para superficie más mate
	animShader.setVec3("light.ambient", ambientColor);
	animShader.setVec3("light.diffuse", diffuseColor);
	animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	animShader.setVec3("light.direction", lightDirection);
	animShader.setVec3("viewPos", camera.Position);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));	
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);
		
	float michelleBaseX = -1100.0f;
	float michelleY = 0.0f;
	float michelleZ = 50.0f;
	float michelleScale = 2.0f;
	
	static bool printedMichelle = false;
	if (!printedMichelle) {
		std::cout << "\n=== POSICION INICIAL DE MICHELLE ===" << std::endl;
		std::cout << "X base: " << michelleBaseX << " (posicion base)" << std::endl;
		std::cout << "Y: " << michelleY << " (altura)" << std::endl;
		std::cout << "Z: " << michelleZ << " (profundidad)" << std::endl;
		std::cout << "Scale: " << michelleScale << std::endl;
		std::cout << "Rango movimiento X: 0 a " << michelleRangeMax << std::endl;
		std::cout << "Velocidad: " << michelleSpeed << std::endl;
		std::cout << "Camara inicial: (0, 200, 800)" << std::endl;
		std::cout << "====================================\n" << std::endl;
		printedMichelle = true;
	}
	
	// Renderizar Michelle con posición y rotación animada
	modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(michelleBaseX + michelleMovX, michelleY, michelleZ + michelleMovZ));
	modelOp = glm::rotate(modelOp, glm::radians(michelleRotation), glm::vec3(0.0f, 1.0f, 0.0f));
	modelOp = glm::scale(modelOp, glm::vec3(michelleScale));
	animShader.setMat4("model", modelOp);
	caminaMichelle.Draw(animShader);


		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

	modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
	modelOp = glm::rotate(modelOp, glm::radians(-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelOp = glm::scale(modelOp, glm::vec3(60.0f)); 
	staticShader.setMat4("model", modelOp);
	escenario.Draw(staticShader);
	renderDrone(drone, staticShader, droneAnim);
	renderBusto(busto, staticShader, bustoAnim);
	// Orden: cuerpo, cola, pataDelDer, pataDelIzq, pataTraDer, pataTraIzq
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
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
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
	// SISTEMA DE AUDIO - Control por teclado
	// ========================================================================
	// Tecla Q - Mover cámara Y reproducir audio de ambiente urbano
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		glm::vec3 newPos = glm::vec3(-3000.0f, 0.0f, -1000.0f);
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 newFront = glm::normalize(target - newPos);

		camera.Position = newPos;
		camera.Front = newFront;
		camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
		camera.Yaw = glm::degrees(atan2(camera.Front.z, camera.Front.x));
		camera.Pitch = glm::degrees(asin(camera.Front.y));
		
		PlayAudioTrack(-1);  // Loop infinito
	}

	// Tecla E - Resetear camara Y detener reproduccion de audio
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		// Función original: Resetear cámara
		camera.Position = glm::vec3(0.0f, 200.0f, 800.0f);
		camera.Yaw = -90.0f;
		camera.Pitch = 0.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		front.y = sin(glm::radians(camera.Pitch));
		front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		camera.Front = glm::normalize(front);
		camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
		
		// Nueva función: Detener audio
		StopAudio();
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

void renderDrone(Model& droneModel, Shader& shader, const DroneAnimation& anim) {
	glm::vec3 position = calculateDronePosition(anim);
	
	glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
	model = glm::rotate(model, anim.currentAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(anim.scale));
	
	shader.setMat4("model", model);
	droneModel.Draw(shader);
}

void renderPerro(Model& cuerpo, Model& cola, Model& pataDerDel, Model& pataIzqDel, 
                 Model& pataDerTra, Model& pataIzqTra, Shader& shader) {
	float perroX = -5000.0f;
	float perroY = -1200.0f;
	float perroZ = -5000.0f;
	float perroScale = 50.0f;
	float perroRotY = -20.0f;
	
	glm::mat4 modelBase = glm::translate(glm::mat4(1.0f), glm::vec3(perroX, perroY, perroZ));
	modelBase = glm::rotate(modelBase, glm::radians(perroRotY), glm::vec3(0.0f, 1.0f, 0.0f));
	modelBase = glm::scale(modelBase, glm::vec3(perroScale));
	
	shader.setMat4("model", modelBase);
	pataDerDel.Draw(shader);
	shader.setMat4("model", modelBase);
	pataIzqDel.Draw(shader);
	shader.setMat4("model", modelBase);
	pataDerTra.Draw(shader);
	shader.setMat4("model", modelBase);
	pataIzqTra.Draw(shader);
	shader.setMat4("model", modelBase);
	cola.Draw(shader);
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