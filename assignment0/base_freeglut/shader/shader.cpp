#include <iostream>
#include <sstream>
#include <fstream>
#include "shader.h"
#include "shader.hpp"
#include <glm/glm.hpp>
using namespace std;
using namespace glm;

int frame = 0;
float dt = 0.0f;
int curFPS = 0;
// std::chrono::_V2::steady_clock::time_point lastT;

int relX, relY, orgX, orgY;
bool wDown, aDown, sDown, dDown, spDown, shDown, enDown;
bool mouseDown;

// scene objects
/* ----- FLUID PLANE ----- */
Shader *advStep, *frcStep, *difStep, *divStep, *prsStep, *grdStep;
TexturePair *vel1, *vel2, *tmp, *qnt1, *qnt2, *prs1, *prs2;
TexturePair *curVel, *nxtVel, *curQnt, *nxtQnt, *curPrs, *nxtPrs;

Shader* fluidShader;


void initShader()
{
    wDown = false; aDown = false; sDown = false; dDown = false; spDown = false; shDown = false; enDown = false;
    mouseDown = false;

    curPrs = NULL; curVel = NULL; curQnt = NULL;
    nxtPrs = NULL; nxtVel = NULL; nxtQnt = NULL;

    advStep = 0;

    dt = 1;

}



void setShader(unsigned int shader) {
    glm::vec2 res = glm::vec2(800, 600);
    // glm::vec2 mpos = glm::vec2(orgX, res.y - orgY);
    // glm::vec2 rel = glm::vec2(relX, -relY);
    // int mDown = mouseDown;

    glUseProgram(shader); 

    glUniform1f(glGetUniformLocation(shader, "dt"), dt); 
    glUniform2fv(glGetUniformLocation(shader, "res"), 1, &res[0]); 
    // glUniform2fv(glGetUniformLocation(shader, "mpos"), 1, &mpos[0]); 
    // glUniform2fv(glGetUniformLocation(shader, "rel"), 1, &rel[0]); 
    // glUniform1i(glGetUniformLocation(shader, "frame"), frame);
    // glUniform1i(glGetUniformLocation(shader, "mDown"), mDown);

    glUniform1i(glGetUniformLocation(shader, "velTex"), 0);
    glUniform1i(glGetUniformLocation(shader, "tmpTex"), 1); 
    glUniform1i(glGetUniformLocation(shader, "prsTex"), 2); 
    glUniform1i(glGetUniformLocation(shader, "qntTex"), 3); 
 

}

void advectionStep() {
    // Activate shader program
    setShader(advStep);

    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, nxtQnt->FBO);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set texture units and bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, curVel->TEX);
    glUniform1i(glGetUniformLocation(advStep, "curVel"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tmp->TEX);
    glUniform1i(glGetUniformLocation(advStep, "tmp"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, curPrs->TEX);
    glUniform1i(glGetUniformLocation(advStep, "curPrs"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, curQnt->TEX);
    glUniform1i(glGetUniformLocation(advStep, "curQnt"), 3);

    // Define quad vertices and texture coordinates
    float vertices[] = {
        // positions         // texCoords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    // Create and bind VAO, VBO, and EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texture coords
    glEnableVertexAttribArray(1);

    // Draw the quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind the framebuffer and VAO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Swap textures
    TexturePair* temp = nxtQnt;
    nxtQnt = curQnt;
    curQnt = temp;
}