#ifndef SHADER_H
#define SHADER_H
#include "../glad.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class TexturePair {
    public:
        TexturePair(int rx, int ry) {
            FBO = 0; TEX = 0;
            setupFBO(rx, ry);
        }

        GLuint FBO, TEX;
    private:
        void setupFBO(int rx, int ry) {
            cout << "Texture Pair class: Inside setupFBO: ";
            glGenFramebuffers(1, &FBO);

            glGenTextures(1, &TEX);
            glBindTexture(GL_TEXTURE_2D, TEX);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, rx, ry, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
            
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX, 0);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            //GLuint clearColor[4] = {0, 0, 0, 0};
            //glClearBufferuiv(GL_COLOR, 0, clearColor);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
                cout << TEX << " " << FBO << "\n";
            } else {
                cout << "nay\n";
            }
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
};

class Shader {
    public:
        unsigned int ID;
        
        Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL) {
            string vertexCode;
            string fragmentCode;
            string geometryCode;
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;
            std::ifstream gShaderFile;
            
            // ensure ifstream objects can throw exceptions:
            vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            
            try {
                // open files
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;
                
                // read file's buffer contents into streams
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();		
                
                // close file handlers
                vShaderFile.close();
                fShaderFile.close();
                
                // convert stream into string
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();			
                
                // if geometry shader path is present, also load a geometry shader
                if(geometryPath != nullptr) {
                    gShaderFile.open(geometryPath);
                    std::stringstream gShaderStream;
                    gShaderStream << gShaderFile.rdbuf();
                    gShaderFile.close();
                    geometryCode = gShaderStream.str();
                }
            } catch (std::ifstream::failure& e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
            }
            
            const char* vShaderCode = vertexCode.c_str();
            const char * fShaderCode = fragmentCode.c_str();
            
            // 2. compile shaders
            unsigned int vertex, fragment;
            
            // vertex shader
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            checkCompileErrors(vertex, "VERTEX");
            
            // fragment Shader
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            checkCompileErrors(fragment, "FRAGMENT");
            
            // if geometry shader is given, compile geometry shader
            unsigned int geometry;
            if(geometryPath != nullptr) {
                const char * gShaderCode = geometryCode.c_str();
                geometry = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(geometry, 1, &gShaderCode, NULL);
                glCompileShader(geometry);
                checkCompileErrors(geometry, "GEOMETRY");
            }
            
            // shader Program
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            if(geometryPath != nullptr)
                glAttachShader(ID, geometry);
            glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");
            
            // delete the shaders as they're linked into our program now and no longer necessery
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            if(geometryPath != nullptr)
                glDeleteShader(geometry);
        }

        void use() { 
            glUseProgram(ID); 
        }
        
        void setBool(const std::string &name, bool value) const {         
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
        }
        
        void    setInt(const std::string &name, int value) const { 
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
        }
        
        void setFloat(const std::string &name, float value) const { 
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
        }
        
        void setVec2(const std::string &name, const glm::vec2 &value) const { 
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        void setVec2(const std::string &name, float x, float y) const { 
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
        }
        
        void setVec3(const std::string &name, const glm::vec3 &value) const { 
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        void setVec3(const std::string &name, float x, float y, float z) const { 
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
        }
        
        void setVec4(const std::string &name, const glm::vec4 &value) const { 
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        void setVec4(const std::string &name, float x, float y, float z, float w)  { 
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
        }
        
        void setMat2(const std::string &name, const glm::mat2 &mat) const {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        
        void setMat3(const std::string &name, const glm::mat3 &mat) const {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        
        void setMat4(const std::string &name, const glm::mat4 &mat) const {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    private:
        void checkCompileErrors(GLuint shader, string type) {
            GLint success;
            GLchar infoLog[1024];
            if(type != "PROGRAM") {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if(!success) {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
                }
            }
            else {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if(!success) {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
                }
            }
        }
};



class HomogenousFluidShader{
    public:

    int frame;
    float dt;
    int curFPS;

    double lastX, lastY;
    int relX, relY;
    bool wDown, aDown, sDown, dDown, spDown, shDown, enDown;
    bool mouseDown;
    double mouseX, mouseY;

    // scene objects
    /* ----- FLUID PLANE ----- */
    unsigned int advStep, frcStep;
    int height, width;
    // Shader *frcStep, *difStep, *divStep, *prsStep, *grdStep;
    TexturePair *vel1, *vel2, *tmp, *qnt1, *qnt2, *prs1, *prs2;
    TexturePair *curVel, *nxtVel, *curQnt, *nxtQnt, *curPrs, *nxtPrs;




    HomogenousFluidShader(int width, int height)
    {
        wDown = false; aDown = false; sDown = false; dDown = false; spDown = false; shDown = false; enDown = false;
        mouseDown = false;

        curPrs = NULL; curVel = NULL; curQnt = NULL;
        nxtPrs = NULL; nxtVel = NULL; // nxtQnt = NULL;
        this->height = height;
        this->width = width;

        advStep = 0;
        frcStep = 0;
        nxtQnt = new TexturePair(width, height);
        curVel = new TexturePair(width, height);
        nxtVel = new TexturePair(width, height);
        tmp = new TexturePair(width, height);
        curPrs = new TexturePair(width, height);
        curQnt = new TexturePair(width, height);

        frame = 0;
    
        curFPS = 0;
        dt = 0;
        lastX = 0;
        lastY = 0;
        relX = 0;
        relY = 0;

    }


    void setShader(unsigned int shader) {
        glm::vec2 res = glm::vec2(width, height);
        glm::vec2 mpos = glm::vec2(mouseX, res.y - mouseY);
        glm::vec2 rel = glm::vec2(relX, -relY);
        int mDown = mouseDown;

        glUseProgram(shader); 

        glUniform1f(glGetUniformLocation(shader, "dt"), dt); 
        glUniform2fv(glGetUniformLocation(shader, "res"), 1, &res[0]); 
        glUniform2fv(glGetUniformLocation(shader, "mpos"), 1, &mpos[0]); 
        glUniform2fv(glGetUniformLocation(shader, "rel"), 1, &rel[0]); 
        glUniform1i(glGetUniformLocation(shader, "frame"), frame);
        glUniform1i(glGetUniformLocation(shader, "mDown"), mDown);

        glUniform1i(glGetUniformLocation(shader, "velTex"), 0);
        glUniform1i(glGetUniformLocation(shader, "tmpTex"), 1); 
        glUniform1i(glGetUniformLocation(shader, "prsTex"), 2); 
        glUniform1i(glGetUniformLocation(shader, "qntTex"), 3); 
    

    }

    void advectionStep() {
        // Activate shader program
        setShader(advStep);

        // // Bind the framebuffer
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

    void forceStep(){
        setShader(frcStep);
    
        // glBindFramebuffer(GL_FRAMEBUFFER, nxtVel->FBO);

        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, curVel->TEX);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tmp->TEX);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, curPrs->TEX);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, curQnt->TEX);

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
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(0);

        TexturePair* temp = nxtVel;
        nxtVel = curVel;
        curVel = temp;

    }


};


#endif
