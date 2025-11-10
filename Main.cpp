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
// #include <mmsystem.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);
void processInput(GLFWwindow* window);

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

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f,
giro = 0.0f,
my_angle = 0.0f;

//Texture
unsigned int	t_smile,
t_toalla,
t_unam,
t_white,
t_ladrillos;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(-1.0f, 0.0f, 0.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// posiciones
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 90.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;

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

// Ruta del archivo de audio
const char* audioFile = "resources/audio/Sonido de calle, trafico, carros, ruidos de ciudad.mp3";

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
	//printf("frameindex %d\n", FrameIndex);
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
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	
	if(isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


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
// InitAudio - Inicializa el sistema de audio SDL_mixer (Nueva API SDL3)
// ============================================================================
void InitAudio() {
    std::cout << "=== Inicializando Sistema de Audio ===" << std::endl;
    
    // Paso 1: Inicializar SDL_mixer primero
    if (!MIX_Init()) {
        std::cerr << "ERROR: No se pudo inicializar SDL_mixer" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return;
    }
    std::cout << "[OK] SDL_mixer inicializado" << std::endl;
    
    // Paso 2: Configurar formato de audio
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_S16;  // 16-bit signed
    spec.channels = 2;            // Estéreo
    spec.freq = 44100;            // 44.1kHz (calidad CD)
    
    // Paso 3: Verificar que SDL esté inicializado
    // SDL debería haberse inicializado al inicio de main()
    Uint32 initialized = SDL_WasInit(SDL_INIT_AUDIO);
    if (!(initialized & SDL_INIT_AUDIO)) {
        std::cerr << "ERROR: SDL (audio) no está inicializado" << std::endl;
        std::cerr << "       SDL debería haberse inicializado al inicio del programa" << std::endl;
        std::cerr << "\n[INFO] El programa continuará sin funcionalidad de audio" << std::endl;
        MIX_Quit();
        return;
    }
    std::cout << "[OK] SDL (audio) está inicializado" << std::endl;
    
    // Paso 4: Intentar abrir un dispositivo de audio directamente para verificar que funciona
    // Esto nos ayudará a diagnosticar si el problema es con SDL o con SDL_mixer
    // SDL_AUDIO_DEVICE_DEFAULT_OUTPUT = 0xFFFFFFFF según SDL_audio.h
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
        std::cerr << "\n[INFO] El programa continuará sin funcionalidad de audio" << std::endl;
        MIX_Quit();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
    std::cout << "[OK] Dispositivo de audio abierto correctamente (ID: " << testDevice << ")" << std::endl;
    SDL_CloseAudioDevice(testDevice);
    
    // Paso 5: Crear mixer con dispositivo predeterminado
    // Usar DEFAULT_OUTPUT (0xFFFFFFFF) en lugar de 0
    std::cout << "[INFO] Creando mixer de audio..." << std::endl;
    SDL_ClearError();
    audioSystem.mixer = MIX_CreateMixerDevice(DEFAULT_OUTPUT, &spec);
    if (!audioSystem.mixer) {
        const char* errorMsg = SDL_GetError();
        std::cerr << "ERROR: No se pudo crear el mixer de audio" << std::endl;
        if (errorMsg && strlen(errorMsg) > 0) {
            std::cerr << "       Error: " << errorMsg << std::endl;
        }
        std::cerr << "\n[INFO] El programa continuará sin funcionalidad de audio" << std::endl;
        MIX_Quit();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
    std::cout << "[OK] Mixer de audio creado (44.1kHz, Estéreo)" << std::endl;
    
    // Paso 6: Cargar archivo de audio
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
    
    // Paso 7: Crear track para reproducir
    audioSystem.track = MIX_CreateTrack(audioSystem.mixer);
    if (!audioSystem.track) {
        std::cerr << "ERROR: No se pudo crear el track de audio" << std::endl;
        MIX_DestroyMixer(audioSystem.mixer);
        audioSystem.mixer = nullptr;
        MIX_Quit();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
    
    // Configurar volumen inicial (0.0 a 1.0)
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
    
    // Detener track actual si está reproduciéndose (0 = sin fade out)
    if (MIX_TrackPlaying(audioSystem.track)) {
        MIX_StopTrack(audioSystem.track, 0);
    }
    
    // Asignar audio al track
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
    
    // Las propiedades se liberan automáticamente por MIX_PlayTrack cuando se consumen
    
    audioSystem.isPlaying = true;
    
    std::cout << "[AUDIO] Reproduciendo audio de ambiente urbano" 
              << (loops == -1 ? " (loop infinito)" : "") << std::endl;
}

// ============================================================================
// StopAudio - Detiene la reproducción actual
// ============================================================================
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

// ============================================================================
// CleanupAudio - Libera recursos de audio y cierra SDL_mixer
// ============================================================================
void CleanupAudio() {
    if (!audioSystem.initialized) {
        return;
    }
    
    std::cout << "\n=== Limpiando Sistema de Audio ===" << std::endl;
    
    // Paso 1: Detener reproducción
    if (audioSystem.track && MIX_TrackPlaying(audioSystem.track)) {
        MIX_StopTrack(audioSystem.track, 0);  // 0 = sin fade out
        std::cout << "[OK] Reproducción detenida" << std::endl;
    }
    
    // Paso 2: Liberar el track
    if (audioSystem.track) {
        // Los tracks se destruyen automáticamente al destruir el mixer
        audioSystem.track = nullptr;
    }
    
    // Paso 3: Liberar el audio cargado
    if (audioSystem.backgroundMusic) {
        // MIX_Audio se libera automáticamente al destruir el mixer
        audioSystem.backgroundMusic = nullptr;
        std::cout << "[OK] Audio liberado" << std::endl;
    }
    
    // Paso 4: Destruir mixer (libera todo)
    if (audioSystem.mixer) {
        MIX_DestroyMixer(audioSystem.mixer);
        audioSystem.mixer = nullptr;
        std::cout << "[OK] Mixer destruido" << std::endl;
    }
    
    // Paso 5: Finalizar SDL_mixer
    MIX_Quit();
    std::cout << "[OK] SDL_mixer finalizado" << std::endl;
    
    // Paso 6: Cerrar subsistema de audio de SDL
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    std::cout << "[OK] SDL (audio) finalizado" << std::endl;
    
    audioSystem.initialized = false;
    std::cout << "===================================\n" << std::endl;
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

	//Veh�culo
	if (animacion)
	{
		movAuto_x += 3.0f;
	}
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
    // CRÍTICO: Configurar hint de categoría de audio para macOS ANTES de inicializar SDL
    // Esto es necesario para que SDL3 funcione correctamente en macOS 15.5 (Sequoia)
    SDL_SetHint(SDL_HINT_AUDIO_CATEGORY, "playback");
    
    // Diagnóstico: Listar drivers de audio disponibles antes de inicializar
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
    
    // CRÍTICO: Inicializar SDL ANTES de GLFW para evitar conflictos
    // En macOS, SDL puede necesitar permisos de audio que se solicitan automáticamente
    SDL_ClearError();
    
    // Intentar inicializar SDL con audio
    // Nota: En macOS, la primera vez que se ejecuta puede aparecer un diálogo pidiendo permisos
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
        
        // Mostrar información del driver actual (si hay alguno)
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

    ModelAnim animacionPersonaje("resources/objects/Joe/joe.fbx");
	animacionPersonaje.initShaders(animShader.ID);

    // ModelAnim caminaMichelle("resources/objects/Michelle/michelle-walking.dae");
	// caminaMichelle.initShaders(animShader.ID);


	//Inicializaci�n de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}


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

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));	
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
         /*
         modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(90.0f, 0.0f, 30.0f));
         modelOp = glm::scale(modelOp, glm::vec3(0.1f));
         animShader.setMat4("model", modelOp);
         caminaMichelle.Draw(animShader);
		 */



		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		/*myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		//Colocar cdigo aqu
		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_ladrillos);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO[0]);
		//Colocar cdigo aqu
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f, 5.0f, 1.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_unam);
		//glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//   Segundo objeto
		glBindVertexArray(VAO[1]);
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_unam);
		glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		*/
		// ------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------

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

		/*modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, -50.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		//r2d2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, 130.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		//casaBruja.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, -10.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		//caja.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -10.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		//casaDoll.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		//piso.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		//casaVieja.Draw(staticShader);
		*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Carro
		// -------------------------------------------------------------------------------------------------------------------------
		/*
		//modelOp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movAuto_x, -1.0f, movAuto_z - 15.0f));
		tmp = modelOp = glm::rotate(modelOp, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.1f, 0.9f));
		staticShader.setMat4("model", modelOp);
		//carro.Draw(staticShader);

		modelOp = glm::translate(tmp, glm::vec3(8.5f, 2.5f, 12.9f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", modelOp);
		//llanta.Draw(staticShader);	//Izq delantera

		modelOp = glm::translate(tmp, glm::vec3(-8.5f, 2.5f, 12.9f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		//llanta.Draw(staticShader);	//Der delantera

		modelOp = glm::translate(tmp, glm::vec3(-8.5f, 2.5f, -14.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		//llanta.Draw(staticShader);	//Der trasera

		modelOp = glm::translate(tmp, glm::vec3(8.5f, 2.5f, -14.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", modelOp);
		//llanta.Draw(staticShader);	//Izq trase
		*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Aquaman
		// -------------------------------------------------------------------------------------------------------------------------
		/*
		//modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		//staticShader.setMat4("model", modelOp);
		//aquaman.Draw(staticShader);

		glm::mat4 tmp01;
		
		tmp01 = modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 50.0f));
		staticShader.setMat4("model", modelOp);
		aquaCuerpo.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(tmp01), glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		aquaCabeza.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(tmp01), glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		aquaBrazoDerecho.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(tmp01), glm::vec3(0.0f, 0.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(giro), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		aquaBrazoIzquierdo.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(tmp01), glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		aquaPiernaDerecha.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(tmp01), glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		aquaPiernaIzquierda.Draw(staticShader);
		*/
		
		
		
		
		
		
		
		
		// -------------------------------------------------------------------------------------------------------------------------
		// Just in case
		// -------------------------------------------------------------------------------------------------------------------------
		/*modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
		tmp = modelOp = glm::rotate(modelOp, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", modelOp);
		torso.Draw(staticShader);

		//Pierna Der
		modelOp = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelOp = glm::rotate(modelOp, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		piernaDer.Draw(staticShader);

		//Pie Der
		modelOp = glm::translate(modelOp, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", modelOp);
		botaDer.Draw(staticShader);

		//Pierna Izq
		modelOp = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		piernaIzq.Draw(staticShader);

		//Pie Iz
		modelOp = glm::translate(modelOp, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", modelOp);
		botaDer.Draw(staticShader);	//Izq trase

		//Brazo derecho
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(-0.75f, 2.5f, 0));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		brazoDer.Draw(staticShader);

		//Brazo izquierdo
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.75f, 2.5f, 0));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		brazoIzq.Draw(staticShader);

		//Cabeza
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, 2.5f, 0));
		staticShader.setMat4("model", modelOp);
		cabeza.Draw(staticShader);*/

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	skybox.Terminate();
	CleanupAudio();     // Limpiar sistema de audio
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
		// Función original: Mover cámara a posición específica
		glm::vec3 newPos = glm::vec3(-3000.0f, 0.0f, -1000.0f);
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 newFront = glm::normalize(target - newPos);

		camera.Position = newPos;
		camera.Front = newFront;
		// Recalculate the Right and Up vector
		camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
		//Update Yaw and Pitch angles
		camera.Yaw = glm::degrees(atan2(camera.Front.z, camera.Front.x));
		camera.Pitch = glm::degrees(asin(camera.Front.y));
		
		// Nueva función: Reproducir audio
		PlayAudioTrack(-1);  // Loop infinito
	}

	// Tecla E - Resetear cámara Y detener reproducción de audio
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


	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.y++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.y--;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		giro++;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		giro--;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}