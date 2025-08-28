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

		0.52738f, 0.43345f, 0.0f,     1.0f, 1.0f, 1.0f, // h = 0
	    0.79008f, 0.59887f, 0.0f,     1.0f, 1.0f, 1.0f, // d = 1
		0.78693f, 0.55672f, 0.0f,     1.0f, 1.0f, 1.0f, // e = 2
		0.82829f, 0.57011f, 0.0f,     1.0f, 1.0f, 1.0f, // f = 3
		0.77078f, 0.47717f, 0.0f,     1.0f, 1.0f, 1.0f, // g = 4
		0.83531f, 0.35247f, 0.0f,     0.0f, 1.0f, 0.0f, // i = 5
		0.73671f, 0.17715f, 0.0f,     0.0f, 1.0f, 0.0f, // j = 6
		0.95f, 0.25f, 0.0f,           0.0f, 1.0f, 0.0f, // k = 7
		0.871f, 0.1618f, 0.0f,        0.0f, 1.0f, 0.0f,	// l = 8
		0.62858f, 0.1109f, 0.0f,	  1.0f, 1.0f, 1.0f, // c = 9
		0.54306f, -0.05877f, 0.0f,    1.0f, 1.0f, 1.0f, // m = 10
		0.09716f, 0.20318f,0.0f,	  1.0f, 1.0f, 1.0f, // n = 11
		0.44101f, 0.02887f, 0.0f,	  1.0f, 1.0f, 1.0f, // o = 12
		0.74225f, 0.2416f, 0.0f,	  0.0f, 1.0f, 0.0f, // p = 13
		-0.29375f, 0.21326f, 0.0f,    1.0f, 1.0f, 1.0f, // q = 14
		-0.5134f, 0.12199f, 0.0f,     1.0f, 1.0f, 1.0f, // r = 15
		0.77078f, 0.47717f, 0.0f,     0.0f, 1.0f, 0.0f, // g2 = 16
	};

	unsigned int indices[] =
	{
		/*3,0,1,
		4,9,0,
		0,3,4,
		1,2,3,
		5,6,7,
		4,5,7,
		4,9,8*/

        // perro con lines
    	//0,1,2,3,4,5,7,8,6,9,
		//10,12,11,13,14,15,

		// orejas
		2,1,0,4,3,

		// cabeza
		4,0,9,6,13,

		// rostro
		5,16,13,6,8,7
		//I, g,p,j,l,k
		
		

    
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

		glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, 0); // barra superior
		glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, (void*)(10 * sizeof(float))); 
		//glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, (void*)(5 * sizeof(float))); // barra vertical
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(11 * sizeof(unsigned int))); // 2 triángulos * 3 vértices = 6 índices - barra media



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