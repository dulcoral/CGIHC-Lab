
#include <iostream>
#include <vector>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"

// Other includes
#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include "skybox.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_mixer.h>

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Museum", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // GLFW Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize SDL and SDL_mixer
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Load music
    Mix_Music* backgroundMusic = Mix_LoadMUS("resources/sounds/background.mp3");
    if (backgroundMusic == NULL) {
        std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Play music
    Mix_PlayMusic(backgroundMusic, -1);

    // Build and compile our shader program
    Shader ourShader("shaders/shader_Lights.vs", "shaders/shader_Lights.fs");

    // Load models
    Model ourModel("resources/objects/Caja/cajaTextura.obj");
    Model Casa("resources/objects/Casa/casa.obj");
    Model Lambo("resources/objects/Lambo/Avent.obj");
    Model R2D2("resources/objects/R2D2/r2-d2.obj");
    Model Aquaman("resources/objects/Aquaman/Aquaman.obj");
    Model Mimikyu("resources/objects/Mimikyu/Mimikyu.obj");
    Model Zombie("resources/objects/ZombieWalk/ZombieWalk.dae");

    // Skybox
    Skybox skybox;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        DoMovement();

        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI window
        ImGui::Begin("Museum Info");
        ImGui::Text("Welcome to the virtual museum!");
        ImGui::Text("Use W, A, S, D or arrow keys to move around.");
        ImGui::Text("Use the mouse to look around.");
        ImGui::End();
       
        ourShader.Use();
        lightPos = camera.GetPosition();

        glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        
        // Set the light uniforms
        glUniform3f(glGetUniformLocation(ourShader.Program, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(ourShader.Program, "viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        // Light properties
        glUniform3f(glGetUniformLocation(ourShader.Program, "light.ambient"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(ourShader.Program, "light.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(ourShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
        // Material properties
        glUniform1f(glGetUniformLocation(ourShader.Program, "material.shininess"), 64.0f);

        //Floor
        glm::mat4 model(1);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 0.01f, 10.0f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        ourModel.Draw(ourShader);
        
        //Casa
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.0f, -0.99f, 0.0f));
        model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        Casa.Draw(ourShader);

        //Lambo
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(2.0f, -0.99f, 2.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        Lambo.Draw(ourShader);

        //R2D2
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-2.0f, -0.99f, -2.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        R2D2.Draw(ourShader);

        //Aquaman
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.0f, -0.99f, -3.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        Aquaman.Draw(ourShader);

        //Mimikyu
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-3.0f, -0.99f, 3.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        Mimikyu.Draw(ourShader);

        //Zombie
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.0f, -0.99f, 4.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        Zombie.Draw(ourShader);
        
        // Draw skybox as last
        skybox.Draw(view, projection);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    Mix_FreeMusic(backgroundMusic);
    Mix_Quit();
    SDL_Quit();
    glfwTerminate();

    return EXIT_SUCCESS;
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

bool CheckCollision(const glm::vec3& position)
{
    // Limites del museo
    if (position.x < -4.5f || position.x > 4.5f || position.z < -4.5f || position.z > 4.5f)
        return true;
    return false;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
    // Camera controls
    glm::vec3 oldPos = camera.GetPosition();
    glm::vec3 newPos = oldPos;

    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        newPos += camera.GetFront() * deltaTime * 5.0f;
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        newPos -= camera.GetFront() * deltaTime * 5.0f;
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        newPos -= camera.GetRight() * deltaTime * 5.0f;
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        newPos += camera.GetRight() * deltaTime * 5.0f;
    }

    if (!CheckCollision(newPos))
    {
        camera.SetPosition(newPos);
    }
}
