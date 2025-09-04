/*---------------------------------------------------------*/
/* ----------------   Pr�ctica 2 --------------------------*/
/*-----------------    2026-1   ---------------------------*/
/*------------- Dulce Coral Rodriguez Garcia ---------------*/
#include<iostream>

//#define GLEW_STATIC

#include <GL/glew.h>

#include <GLFW/glfw3.h>

void resize(GLFWwindow* window, int width, int height);
void my_input(GLFWwindow *window);

// settings
// Window size
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

GLFWmonitor *monitors;
GLuint VBO[2], VAO[2], EBO[2];
GLuint shaderProgramYellow, shaderProgramColor;

static const char* myVertexShader = "										\n\
#version 330 core															\n\
																			\n\
layout (location = 0) in vec3 aPos;											\n\
																			\n\
void main()																	\n\
{																			\n\
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);							\n\
}";

static const char* myVertexShaderColor = "									\n\
#version 330 core															\n\
																			\n\
layout (location = 0) in vec3 aPos;											\n\
layout (location = 1) in vec3 aColor;										\n\
out vec3 ourColor;															\n\
void main()																	\n\
{																			\n\
    gl_Position = vec4(aPos, 1.0);											\n\
	ourColor = aColor;														\n\
}";

// Fragment Shader
static const char* myFragmentShaderYellow = "									\n\
#version 330																\n\
																			\n\
out vec3 finalColor;														\n\
																			\n\
void main()																	\n\
{																			\n\
    finalColor = vec3(1.0f, 0.0f, 0.0f);									\n\
}";

static const char* myFragmentShaderColor = "								\n\
#version 330 core															\n\
out vec4 FragColor;															\n\
in vec3 ourColor;															\n\
																			\n\
void main()																	\n\
{																			\n\
	FragColor = vec4(ourColor, 1.0f);										\n\
}";

void myData(void);
void setupShaders(void);
void display(void);
void getResolution(void);


void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData()
{
	float vertices[] = 
	{
		// positions   XYZ      // color
		/*-0.7f, 0.8f, 0.0f,    1.0f, 1.0f, 1.0f,
		0.3f, 0.8f, 0.0f,     1.0f, 1.0f, 0.0f,
	    0.3f, 0.6f, 0.0f,     1.0f, 0.0f, 1.0f,
		0.0f, 0.6f, 0.0f,     0.0f, 1.0f, 1.0f,
		0.0f, 0.4f, 0.0f,     0.0f, 0.0f, 1.0f,
		0.15f, 0.4f, 0.0f,    0.0f, 1.0f, 0.0f,
		0.15f, 0.2f, 0.0f,    1.0f, 1.0f, 1.0f,
		0.0f, 0.2f, 0.0f,     0.26f, 0.58f, 0.367f,
		0.0f, -0.4f, 0.0f,    1.0f, 1.0f, 1.0f,
		-0.7f, -0.4f, 0.0f,   1.0f, 1.0f, 1.0f,	*/

		0.10213f, 0.11903f, 0.0f,    0.87f, 0.72f, 0.53f,  // c = 0 (cafe dorado)
		0.55427f, 0.35793f, 0.0f,    0.87f, 0.72f, 0.53f,  // d = 1
		0.82071f, 0.5259f, 0.0f,     1.0f, 0.5f, 0.0f,   // e = 2 (naranja)
		0.82205f, 0.48947f, 0.0f,    1.0f, 0.5f, 0.0f,   // f = 3 (naranja)
		0.85493f, 0.49658f, 0.0f,    1.0f, 0.5f, 0.0f,   // g = 4 (naranja)
		0.80561f, 0.40816f, 0.0f,    0.87f, 0.72f, 0.53f,  // h = 5
		0.87254f, 0.27944f, 0.0f,    0.87f, 0.72f, 0.53f,  // i = 6
		0.98644f, 0.16747f, 0.0f,    0.87f, 0.72f, 0.53f,  // j = 7
		0.91115f, 0.07867f, 0.0f,    0.87f, 0.72f, 0.53f,  // k = 8
		0.76926f, 0.09508f, 0.0f,    0.87f, 0.72f, 0.53f,  // l = 9
		0.77698f, 0.1723f, 0.0f,     0.87f, 0.72f, 0.53f,  // m = 10
		0.66019f, 0.02172f, 0.0f,    0.87f, 0.72f, 0.53f,  // n = 11
		0.57308f, -0.14715f, 0.0f,   0.87f, 0.72f, 0.53f,  // o = 12
		0.46058f, -0.06671f, 0.0f,   0.87f, 0.72f, 0.53f,  // p = 13
		0.40221f, -0.25714f, 0.0f,   0.87f, 0.72f, 0.53f,  // q = 14
		0.23837f, -0.35895f, 0.0f,   0.87f, 0.72f, 0.53f,  // r = 15
		-0.01434f, -0.3372f, 0.0f,   0.87f, 0.72f, 0.53f,  // s = 16
		-0.14488f, -0.28532f, 0.0f,  0.87f, 0.72f, 0.53f,  // t = 17
		-0.2453f, -0.20833f, 0.0f,   0.87f, 0.72f, 0.53f,  // u = 18
		-0.34738f, -0.12465f, 0.0f,  0.87f, 0.72f, 0.53f,  // v = 19
		-0.52502f, 0.03232f, 0.0f,   0.87f, 0.72f, 0.53f,  // w = 20
		-0.29824f, 0.12025f, 0.0f,   0.87f, 0.72f, 0.53f,  // z = 21
		-0.28713f, -0.30205f, 0.0f,  0.87f, 0.72f, 0.53f,  // a1 = 22
		-0.42771f, -0.4828f, 0.0f,   0.87f, 0.72f, 0.53f,  // b1 = 23
		-0.61382f, -0.5025f, 0.0f,   0.87f, 0.72f, 0.53f,  // c1 = 24
		-0.38941f, -0.23006f, 0.0f,  0.87f, 0.72f, 0.53f,  // d1 = 25
		-0.61588f, -0.25751f, 0.0f,  0.87f, 0.72f, 0.53f,  // e1 = 26
		-0.71312f, -0.33438f, 0.0f,  0.87f, 0.72f, 0.53f,  // f1 = 27
		-0.95694f, -0.27839f, 0.0f,  0.87f, 0.72f, 0.53f,  // g1 = 28
		-0.9406f, -0.19089f, 0.0f,   0.87f, 0.72f, 0.53f,  // h1 = 29
		-0.80642f, -0.20904f, 0.0f,  0.87f, 0.72f, 0.53f,  // i1 = 30
		-0.6625f, -0.12435f, 0.0f,   0.87f, 0.72f, 0.53f,  // j1 = 31
		-0.71644f, -0.60133f, 0.0f,  0.87f, 0.72f, 0.53f,  // k1 = 32
		-0.74782f, -0.86267f, 0.0f,  0.87f, 0.72f, 0.53f,  // l1 = 33
		-0.66233f, -0.86213f, 0.0f,  0.87f, 0.72f, 0.53f,  // m1 = 34
		-0.68289f, -0.82046f, 0.0f,  0.87f, 0.72f, 0.53f,  // n1 = 35
		-0.66125f, -0.71008f, 0.0f,  0.87f, 0.72f, 0.53f,  // o1 = 36
		-0.61309f, -0.65002f, 0.0f,  0.87f, 0.72f, 0.53f,  // p1 = 37
		-0.56602f, -0.58888f, 0.0f,  0.87f, 0.72f, 0.53f,  // q1 = 38
		-0.51407f, -0.54776f, 0.0f,  0.87f, 0.72f, 0.53f,  // r1 = 39
		-0.55303f, -0.66517f, 0.0f,  0.87f, 0.72f, 0.53f,  // s1 = 40
		-0.54563f, -0.75517f, 0.0f,  0.87f, 0.72f, 0.53f,  // t1 = 41
		-0.48609f, -0.78472f, 0.0f,  0.87f, 0.72f, 0.53f,  // u1 = 42
		-0.51086f, -0.81384f, 0.0f,  0.87f, 0.72f, 0.53f,  // v1 = 43
		-0.60474f, -0.78472f, 0.0f,  0.87f, 0.72f, 0.53f,  // w1 = 44
		0.28688f, -0.80794f, 0.0f,   0.87f, 0.72f, 0.53f,  // z1 = 45
		0.38988f, -0.81356f, 0.0f,   0.87f, 0.72f, 0.53f,  // a2 = 46
		0.34606f, -0.71918f, 0.0f,   0.87f, 0.72f, 0.53f,  // b2 = 47
		0.36962f, -0.34723f, 0.0f,   0.87f, 0.72f, 0.53f,  // c2 = 48
		0.41612f, -0.32796f, 0.0f,   0.87f, 0.72f, 0.53f,  // d2 = 49
		0.41802f, -0.50319f, 0.0f,   0.87f, 0.72f, 0.53f,  // e2 = 50
		0.40086f, -0.61751f, 0.0f,   0.87f, 0.72f, 0.53f,  // f2 = 51
		0.44091f, -0.68633f, 0.0f,   0.87f, 0.72f, 0.53f,  // g2 = 52
		0.47851f, -0.71036f, 0.0f,   0.87f, 0.72f, 0.53f,  // h2 = 53
		0.47753f, -0.75773f, 0.0f,   0.87f, 0.72f, 0.53f,  // i2 = 54
		0.44019f, -0.7617f, 0.0f,    0.87f, 0.72f, 0.53f,  // j2 = 55
		0.55427f, 0.35793f, 0.0f,    1.0f, 0.5f, 0.0f,   // d_oreja = 56 (naranja)
		0.80561f, 0.40816f, 0.0f,    1.0f, 0.5f, 0.0f,   // h_oreja = 57 (naranja)
		// Vertices duplicados para pata trasera principal (blanco-beige)
		-0.42771f, -0.4828f, 0.0f,   0.96f, 0.91f, 0.84f,  // b1_pata = 58
		-0.61382f, -0.5025f, 0.0f,   0.96f, 0.91f, 0.84f,  // c1_pata = 59
		-0.71644f, -0.60133f, 0.0f,  0.96f, 0.91f, 0.84f,  // k1_pata = 60
		-0.74782f, -0.86267f, 0.0f,  0.96f, 0.91f, 0.84f,  // l1_pata = 61
		-0.66233f, -0.86213f, 0.0f,  0.96f, 0.91f, 0.84f,  // m1_pata = 62
		-0.68289f, -0.82046f, 0.0f,  0.96f, 0.91f, 0.84f,  // n1_pata = 63
		-0.66125f, -0.71008f, 0.0f,  0.96f, 0.91f, 0.84f,  // o1_pata = 64
		-0.61309f, -0.65002f, 0.0f,  0.96f, 0.91f, 0.84f,  // p1_pata = 65
		-0.56602f, -0.58888f, 0.0f,  0.96f, 0.91f, 0.84f,  // q1_pata = 66
		-0.51407f, -0.54776f, 0.0f,  0.96f, 0.91f, 0.84f,  // r1_pata = 67
		// Vertices duplicados para cola (rosa degradado)
		-0.2453f, -0.20833f, 0.0f,   1.0f, 0.8f, 0.9f,   // u_cola = 68 (rosa claro)
		-0.34738f, -0.12465f, 0.0f,  1.0f, 0.75f, 0.85f, // v_cola = 69 (rosa)
		-0.52502f, 0.03232f, 0.0f,   1.0f, 0.7f, 0.8f,   // w_cola = 70 (rosa)
		-0.38941f, -0.23006f, 0.0f,  1.0f, 0.65f, 0.75f, // d1_cola = 71 (rosa medio)
		-0.61588f, -0.25751f, 0.0f,  1.0f, 0.6f, 0.7f,   // e1_cola = 72 (rosa medio)
		-0.71312f, -0.33438f, 0.0f,  1.0f, 0.55f, 0.65f, // f1_cola = 73 (rosa intenso)
		-0.95694f, -0.27839f, 0.0f,  1.0f, 0.5f, 0.6f,   // g1_cola = 74 (rosa intenso)
		-0.9406f, -0.19089f, 0.0f,   1.0f, 0.45f, 0.55f, // h1_cola = 75 (rosa más intenso)
		-0.80642f, -0.20904f, 0.0f,  1.0f, 0.7f, 0.8f,   // i1_cola = 76 (rosa)
		-0.6625f, -0.12435f, 0.0f,   1.0f, 0.75f, 0.85f,  // j1_cola = 77 (rosa)
		// Vertices duplicados para pata delantera (amarillo suave)
		0.40221f, -0.25714f, 0.0f,   1.0f, 1.0f, 0.7f,   // q_pata = 78 (amarillo suave)
		0.23837f, -0.35895f, 0.0f,   1.0f, 1.0f, 0.7f,   // r_pata = 79 (amarillo suave)
		0.28688f, -0.80794f, 0.0f,   1.0f, 1.0f, 0.7f,   // z1_pata = 80 (amarillo suave)
		0.38988f, -0.81356f, 0.0f,   1.0f, 1.0f, 0.7f,   // a2_pata = 81 (amarillo suave)
		0.34606f, -0.71918f, 0.0f,   1.0f, 1.0f, 0.7f,   // b2_pata = 82 (amarillo suave)
		0.36962f, -0.34723f, 0.0f,   1.0f, 1.0f, 0.7f    // c2_pata = 83 (amarillo suave)
	};

	unsigned int indices[] =
	{
		// Primera oreja (triangle_fan desde D)
		56, 2, 3, 4,    // d_oreja(pivote), e, f, g
        
		// Segunda oreja (triangle_fan desde H)
		57, 4, 3, 56,    // h_oreja(pivote), g, f, d_oreja

		// CABEZA (pivote D=1)
		1, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14,

		// TORSO CUELLO — definición por triangulos
		// D->N->O->D
		1, 11, 12,
		// D->O->Q->P->D
		1, 12, 14,
		1, 14, 13,
		// D->P->C->D
		1, 13, 0,
		// C->P->Q->R->C
		0, 13, 14,
		0, 14, 15,
		// C->R->S->T->C
		0, 15, 16,
		0, 16, 17,
		// C->T->U->V->C
		0, 17, 18,
		0, 18, 19,
		// C->V->W->Z->C
		0, 19, 20,
		0, 20, 21,

		// PATA DELANTERA - O-Q_pata-C2_pata-D2 (dos triángulos)
		12, 78, 83,
		12, 83, 49,

		// FAN superior con pivote C2_pata: C2_pata,Q_pata,R_pata,Z1_pata,B2_pata
		83, 78, 79, 80, 82,
		// Triángulo independiente para cerrar Z1_pata-A2_pata-B2_pata
		82, 80, 81,
		// PIE: 7 triángulos especificados (color base)
		// D2-C2-E2
		49, 48, 50,
		// C2-E2-B2
		48, 50, 47,
		// E2-B2-F2
		50, 47, 51,
		// B2-F2-G2
		47, 51, 52,
		// B2-J2-G2
		47, 55, 52,
		// H2-J2-G2
		53, 55, 52,
		// H2-J2-I2
		53, 55, 54,

		// COLA: V,U,D1 y E1,V,D1 - color base; resto rosa degradado
		// V(19), U(18), D1(25) - color base
		19, 18, 25,
		// E1(26), V(19), D1(25) - color base
		26, 19, 25,
		// E1(26), V(19), W(20) - color base
		26, 19, 20,
		// FAN cola con pivote E1_cola: E1_cola, W_cola, J1_cola, I1_cola
		72, 70, 77, 76,
		// Segmentos restantes de la cola
		// E1_cola-I1_cola-F1_cola
		72, 76, 73,
		// F1_cola-I1_cola-G1_cola
		73, 76, 74,
		// I1_cola-G1_cola-H1_cola
		76, 74, 75

		,
		// Muslo y Panza
		// Triangulo E1-C1-D1  => 26,24,25
		26, 24, 25,
		// Triangulo C1-B1-A1  => 24,23,22
		24, 23, 22,
		// PANZA: fan con pivote A1 (A1, D1, U, T) y triangulo C1-D1-A1
		// A1(22), D1(25), U(18), T(17)
		22, 25, 18, 17,
		// C1(24), D1(25), A1(22)
		24, 25, 22,
		// PATA TRASERA PRINCIPAL
		// Fan con pivote K1_pata: K1_pata, C1_pata, B1_pata, R1_pata, Q1_pata, P1_pata, O1_pata, N1_pata, L1_pata
		60, 59, 58, 67, 66, 65, 64, 63, 61,
		// Triangulo N1_pata-L1_pata-M1_pata
		63, 61, 62,

		// PATA TRASERA SECUNDARIA
		// Fan con pivote W1: W1, V1, T1, S1, Q1, P1
		44, 43, 41, 40, 38, 37,
		// Triangulos auxiliares
		// Q1-S1-R1
		38, 40, 39,
		// V1-U1-T1
		43, 42, 41
	};

	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // como se organiza la entrada de posicion
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // como se organiza la entrada de color
	glEnableVertexAttribArray(1);

	//Para trabajar con indices (Element Buffer Object)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

void setupShaders()
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &myVertexShader, NULL);
	glCompileShader(vertexShader);

	unsigned int vertexShaderColor = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderColor, 1, &myVertexShaderColor, NULL);
	glCompileShader(vertexShaderColor);

	unsigned int fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderYellow, 1, &myFragmentShaderYellow, NULL);
	glCompileShader(fragmentShaderYellow);

	unsigned int fragmentShaderColor = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderColor, 1, &myFragmentShaderColor, NULL);
	glCompileShader(fragmentShaderColor);


	//Crear el Programa que combina Geometr�a con Color
	shaderProgramYellow = glCreateProgram();
	glAttachShader(shaderProgramYellow, vertexShader);
	glAttachShader(shaderProgramYellow, fragmentShaderYellow);
	glLinkProgram(shaderProgramYellow);

	shaderProgramColor = glCreateProgram();
	glAttachShader(shaderProgramColor, vertexShaderColor);
	glAttachShader(shaderProgramColor, fragmentShaderColor);
	glLinkProgram(shaderProgramColor);
	//Check for errors 

	//ya con el Programa, el Shader no es necesario
	glDeleteShader(vertexShader);
	glDeleteShader(vertexShaderColor);
	glDeleteShader(fragmentShaderYellow);
	glDeleteShader(fragmentShaderColor);

}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Practica 1 2026", NULL, NULL);//Bug?
	if (window == NULL)
	{
		const char* description;
		int code = glfwGetError(&description);
		std::cout << "Failed to create GLFW window. GLFW error code: " << code << ", description: " << (description ? description : "Unknown") << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resize);

	glewInit();
	//glEnable(GL_MULTISAMPLE);
	//Setup Data to use
	myData();
	//To Setup Shaders
	setupShaders();
    
    // render loop
    // While the windows is not closed
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        my_input(window);

        // render
        // Background color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //RGBA
        glClear(GL_COLOR_BUFFER_BIT);

		//Display Section
		glUseProgram(shaderProgramColor);

		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);

		glPointSize(10.0f);
		glLineWidth(10.0f);
		//glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_INT, 0); // LISTO LA LETRA F
		//glDrawArrays(GL_TRIANGLE_FAN, 0, 10);
		//glDrawArrays(GL_POINTS, 3, 1);

		// Primero: cabeza con degradado
		glDrawElements(GL_TRIANGLE_FAN, 11, GL_UNSIGNED_INT, (void*)(8 * sizeof(unsigned int)));

		// Después: orejas en naranja sólido  
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void*)(4 * sizeof(unsigned int)));

		// 3) TRI_TORSO_CUELLO
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(19 * sizeof(unsigned int)));
		// 4) PATA DELANTERA
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(55 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLE_FAN, 5, GL_UNSIGNED_INT, (void*)(61 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(66 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_INT, (void*)(69 * sizeof(unsigned int)));
		// 5) COLA
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, (void*)(90 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void*)(99 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, (void*)(103 * sizeof(unsigned int)));
		// 6) Triángulo E1-C1-D1
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(112 * sizeof(unsigned int)));
		// 7) Triángulo C1-B1-A1
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(115 * sizeof(unsigned int)));
		// 8) PANZA
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void*)(118 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(122 * sizeof(unsigned int)));
		// 9) PATA TRASERA PRINCIPAL
		glDrawElements(GL_TRIANGLE_FAN, 9, GL_UNSIGNED_INT, (void*)(125 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(134 * sizeof(unsigned int)));
		// 10) PATA TRASERA SECUNDARIA
		glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, (void*)(137 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(143 * sizeof(unsigned int)));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(146 * sizeof(unsigned int)));

		glBindVertexArray(0);
		glUseProgram(0);

		//End of Display Section

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  //GLFW_RELEASE
        glfwSetWindowShouldClose(window, true);  //Close
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void resize(GLFWwindow* window, int width, int height)
{
    // Set the Viewport to the size of the created window
    glViewport(0, 0, width, height);
}