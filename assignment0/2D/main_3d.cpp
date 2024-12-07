#include "glad.c"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "mesh.hpp"
#include "util.hpp"
#include "shader_3D/shader_3d.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
unsigned int shaderProgram;
unsigned int VBO, VAO, EBO;
GLFWwindow* window;
std::chrono::_V2::steady_clock::time_point lastT;
Shader* fluidShader;
bool mouseCal;
bool onlyAdvection = false;
bool tillDiffusion = false;
bool addBox = false;



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
    mouseCal = false;
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
    unsigned int fluidVShader = compileShader("shader_3D/fluid_vs.glsl", GL_VERTEX_SHADER);

    unsigned int advectFragShader = compileShader("shader_3D/advStep.glsl", GL_FRAGMENT_SHADER);
    unsigned int forceFragShader = compileShader("shader_3D/forceStep.glsl", GL_FRAGMENT_SHADER);
    unsigned int diffusionFragShader = compileShader("shader_3D/diffusionStep.glsl", GL_FRAGMENT_SHADER);
    unsigned int fluidFragShader = compileShader("shader_3D/fluid_fs.glsl", GL_FRAGMENT_SHADER);
    unsigned int divergenceShader = compileShader("shader_3D/divergence.glsl", GL_FRAGMENT_SHADER);
    unsigned int pressureFragShader = compileShader("shader_3D/pressure.glsl", GL_FRAGMENT_SHADER);
    unsigned int gradientShader = compileShader("shader_3D/gradient.glsl", GL_FRAGMENT_SHADER);
    unsigned int boundaryShader = compileShader("shader_3D/boundary.glsl", GL_FRAGMENT_SHADER);
    unsigned int cubeVShader = compileShader("shader_3D/cube_vs.glsl", GL_VERTEX_SHADER);
    unsigned int cubeFShader = compileShader("shader_3D/cube_fs.glsl", GL_FRAGMENT_SHADER);
    unsigned int boundCubeFShader = compileShader("shader_3D/boundary_cube.glsl", GL_FRAGMENT_SHADER);

    hfShader->advStep = glCreateProgram();
    glAttachShader(hfShader->advStep, fluidVShader);
    glAttachShader(hfShader->advStep, advectFragShader);
    glLinkProgram(hfShader->advStep);

    hfShader->frcStep = glCreateProgram();
    glAttachShader(hfShader->frcStep, fluidVShader);
    glAttachShader(hfShader->frcStep, forceFragShader);
    glLinkProgram(hfShader->frcStep);

    hfShader->diffStep = glCreateProgram();
    glAttachShader(hfShader->diffStep, fluidVShader);
    glAttachShader(hfShader->diffStep, diffusionFragShader);
    glLinkProgram(hfShader->diffStep);

    hfShader->divStep = glCreateProgram();
    glAttachShader(hfShader->divStep, fluidVShader);
    glAttachShader(hfShader->divStep, divergenceShader);
    glLinkProgram(hfShader->divStep);

    hfShader->prsStep = glCreateProgram();
    glAttachShader(hfShader->prsStep, fluidVShader);
    glAttachShader(hfShader->prsStep, pressureFragShader);
    glLinkProgram(hfShader->prsStep);

    hfShader->grdStep = glCreateProgram();
    glAttachShader(hfShader->grdStep, fluidVShader);
    glAttachShader(hfShader->grdStep, gradientShader);
    glLinkProgram(hfShader->grdStep);

    hfShader->fluidStep = glCreateProgram();
    glAttachShader(hfShader->fluidStep, fluidVShader);
    glAttachShader(hfShader->fluidStep, fluidFragShader);
    glLinkProgram(hfShader->fluidStep);

    hfShader->boundaryStep = glCreateProgram();
    glAttachShader(hfShader->boundaryStep, fluidVShader);
    glAttachShader(hfShader->boundaryStep, boundaryShader);
    glLinkProgram(hfShader->boundaryStep);

    hfShader->cubeBoundaryStep = glCreateProgram();
    glAttachShader(hfShader->cubeBoundaryStep, fluidVShader);
    glAttachShader(hfShader->cubeBoundaryStep, boundCubeFShader);
    glLinkProgram(hfShader->cubeBoundaryStep);

    hfShader->cubeStep = glCreateProgram();
    glAttachShader(hfShader->cubeStep, cubeVShader);
    glAttachShader(hfShader->cubeStep, cubeFShader);
    glLinkProgram(hfShader->cubeStep);

    // glm::mat4 xform;
    // float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    // // Create perspective projection matrix
    // mat4 proj = glm::perspective(45.0f, aspect, 0.0f, 100.0f);
    // // Create view transformation matrix
    // mat4 view = glm::translate(mat4(1.0f), vec3(0.0, 0.0, -2.0)); 
    // // mat4 rot = rotate(mat4(1.0f), radians(camCoords.y), vec3(1.0, 0.0, 0.0));
    // // rot = rotate(rot, radians(camCoords.x), vec3(0.0, 1.0, 0.0));
    // xform = proj * view;
    // glm::mat4 xform = glm::mat4(1.0f);               // Identity matrix
    // xform = glm::translate(xform, glm::vec3(0.0f));  // Center it at origin
    // xform = glm::scale(xform, glm::vec3(0.5f)); 

    glm::mat4 orthoProj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 xform = orthoProj * view;

    hfShader->uniXform = xform;

    glDeleteShader(fluidVShader);
    glDeleteShader(advectFragShader);
    glDeleteShader(forceFragShader);
    glDeleteShader(diffusionFragShader);
    glDeleteShader(divergenceShader);
    glDeleteShader(pressureFragShader);
    glDeleteShader(gradientShader);
    glDeleteShader(fluidFragShader);
    glDeleteShader(boundaryShader);
    glDeleteShader(cubeVShader);
    glDeleteShader(cubeFShader);
    glDeleteShader(boundCubeFShader);
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
        ImGui::Begin("ImGui!");
        

      


        if(ImGui::Checkbox("Only Advection", &onlyAdvection)) {
        // Action when the checkbox state changes
            if (onlyAdvection) {
                tillDiffusion = false;
            } 
        }
        if(ImGui::Checkbox("Till Diffusion", &tillDiffusion)) {
        // Action when the checkbox state changes
            if (tillDiffusion) {
                onlyAdvection = false;
            } 
        }

        if(ImGui::Checkbox("Add Box", &addBox)) {
            if(addBox){}
        }

          // A variable to control
        float value = 0.0f;  // Initial value
        float minVal = -1.0f; // Minimum value
        float maxVal = 1.0f; // Maximum value

        // Optional: Slider for visual clarity
        ImGui::Text("Move box left and right");
        if (ImGui::SliderFloat("LR", &value, minVal, maxVal, "%.3f")) {
            // Action when the slider value changes
            cube.center.x = value;
        }

          // A variable to control
        float up = 0.0f;  // Initial value
        float upMax = 1.0f;
        float downMax = -1.0f;
        // Optional: Slider for visual clarity
        ImGui::Text("Move box up and down");
        if (ImGui::SliderFloat("UpDown", &up, upMax, downMax, "%.3f")) {
            // Action when the slider value changes
            cube.center.y = up;
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
        // hfShader
        //     // Reset mouseClicked after processing
        //     // hfShader->mouseDown = false;
        // }

       
    


        hfShader->advectionStep();
        
        if( onlyAdvection == 0){
            hfShader->forceStep();
            hfShader->diffusionStep();
            if(tillDiffusion == 0){

                hfShader->divergenceStep();
                hfShader->pressureStep();
                hfShader->boundaryVelStep(hfShader->curPrs, 1);

                hfShader->gradientStep();
                hfShader->boundaryVelStep(hfShader->curVel, -1.0);
            }
        }
        // hfShader->cubeBoundaryVelStep();
        hfShader->visFluidStep();
        if(addBox == false){
            // std::cout << "inside " << std::endl;
            cube.size = vec3(0, 0, 0);
        }
        else{
            cube.size = vec3(0.2, 0.2, 0.2);
            hfShader->drawCubeStep();
        }


        // hfShader->visualizeGBuffer(hfShader->nxtVel->FBO);
        // hfShader->printTexture(hfShader->curPrs);
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
