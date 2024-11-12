#include "glad.c"
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "mesh.hpp"
#include "util.hpp"
#include "shader/shader.h"
// #include "shader/shader.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
unsigned int shaderProgram;
unsigned int VBO, VAO, EBO;
GLFWwindow* window;
std::chrono::_V2::steady_clock::time_point lastT;
Shader* fluidShader;



HomogenousFluidShader *hfShader;


void init();
int initWindow();
void initTriangle();
void setShaders();
void display();
void cleanup();

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);

void init()
{
    shaderProgram = 0;
    VBO = 0;
    VAO = 0;
    EBO = 0;
    window = 0;
    hfShader = 0;
    lastT = std::chrono::steady_clock::now();

}

int initWindow()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ImGui + GLFW Example", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePositionCallback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return 0;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // glfwGetCursorPos(window, &hfShader->mouseX, &hfShader->mouseY);
        hfShader->mouseDown = true;
    }
    else if (action == GLFW_RELEASE)
    {
        hfShader->mouseDown = false;
        hfShader->relX = hfShader->relY = 0.0; // Reset relative motion when button is released

    }
}

void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (hfShader->mouseDown) {
        // Calculate relative movement
        hfShader->relX = xpos - hfShader->lastX;
        hfShader->relY = ypos - hfShader->lastY;

        // Calculate origin coordinates (position before relative movement)
        hfShader->mouseX = xpos - hfShader->relX;
        hfShader->mouseY = ypos - hfShader->relY;
    }

    // Update last position
    hfShader->lastX = xpos;
    hfShader->lastY = ypos;
}


void initTriangle()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

}

void setShaders()
{
    // initShader();
    hfShader = new HomogenousFluidShader( SCR_WIDTH, SCR_HEIGHT);

    // Load, compile, and link shaders
    unsigned int vertexShader = compileShader("sh_v.glsl", GL_VERTEX_SHADER);
    unsigned int fluidVShader = compileShader("shader/fluid_vs.glsl", GL_VERTEX_SHADER);

    unsigned int fragmentShader = compileShader("sh_f.glsl", GL_FRAGMENT_SHADER);
    unsigned int advectFragShader = compileShader("shader/advStep.glsl", GL_FRAGMENT_SHADER);
    unsigned int forceFragShader = compileShader("shader/forceStep.glsl", GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    hfShader->advStep = glCreateProgram();
    glAttachShader(hfShader->advStep, fluidVShader);
    glAttachShader(hfShader->advStep, advectFragShader);
    glLinkProgram(hfShader->advStep);

    hfShader->frcStep = glCreateProgram();
    glAttachShader(hfShader->frcStep, fluidVShader);
    glAttachShader(hfShader->frcStep, advectFragShader);
    glLinkProgram(hfShader->frcStep);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(advectFragShader);
    glDeleteShader(forceFragShader);
}

void display()
{
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui window with a button
        ImGui::Begin("Hello, ImGui!");
        if (ImGui::Button("Click Me")) {
            std::cout << "Button Clicked!" << std::endl;
        }
        ImGui::End();

        auto curT = std::chrono::steady_clock::now();
        std::chrono::duration<float> diff = curT - lastT;
        lastT = std::chrono::steady_clock::now();
        hfShader->dt = diff.count();

        hfShader->frame += 1;

       // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // // draw our first triangle
        // glUseProgram(shaderProgram);
        // glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // //glDrawArrays(GL_TRIANGLES, 0, 6);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // if (hfShader->mouseDown) {
              
        //     std::cout << "Mouse origin: (" << hfShader->mouseX << ", " << hfShader->mouseY << ")\n";
        //     std::cout << "Mouse relative motion: (" << hfShader->relX << ", " << hfShader->relY << ")\n";
        
        //     // Reset mouseClicked after processing
        //     // hfShader->mouseDown = false;
        // }

        hfShader->advectionStep();
        hfShader->forceStep();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

}

int main() {
    init();

    if( initWindow() == -1 ) return -1;

    setShaders();

    initTriangle();

    display();

    cleanup();

    return 0;
}

void cleanup()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    hfShader = NULL;

    glfwDestroyWindow(window);
    glfwTerminate();

}
