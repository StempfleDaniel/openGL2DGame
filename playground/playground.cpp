#include "playground.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>                                         
using namespace glm;
#include <iostream>
#include <common/shader.hpp>

//new inclued
#define STB_IMAGE_IMPLEMENTATION
#include <common/stb_image.h>
#include <chrono>
#include <vector>
#include <memory>
#include <random>


//enums for state machine behaviour
enum PlayerDirection { front, back, left, right, none};


float x = 0;
float y = 0;

float xRes = 1920;
float yRes = 1080;
bool playerHit = false;
 
bool test = true;
float gameSpeed = 5.0f;

std::vector<vec2> bulletsPos;

class GameObject {
public:
    //some global variables for handling the vertex sbuffer
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint textureSamplerID;
    GLuint texture;
    int width, height;
    int nrChannels = 4;
    unsigned char* data;
    std::vector<unsigned char*> sprites;
    //modified
    GLuint colorbuffer;
    GLfloat matrix;
    GLuint VertexArrayID;
    GLuint vertexbuffer_size;
    glm::mat4 translation;
    float speed;
    float radius;
    bool isActive = true;
    virtual void update(PlayerDirection* userInput, bool* spaceBarPress) = 0;
    virtual void draw() = 0;  
    virtual bool initializeVAOs() = 0;
    virtual bool cleanupVAOs() = 0;
    virtual unsigned char* loadSpriteBasedOnState() = 0;
    virtual void loadAllSpritesIntoData() = 0;
    virtual void checkCollisions() = 0;
   
   

};


class Player : public GameObject {
    int spriteFrequency = 150;
    bool a;
    std::chrono::steady_clock::time_point lastSpriteUpdate;
    int hitpoints;
    PlayerDirection playerDirection;
    bool playerAnimationState;
public:
    Player(int hp, glm::mat4 translation_g) {
        hitpoints = hp;
        translation = translation_g;
        speed = 0.02f;
        radius = 0.1f;
        isActive = true;
        playerDirection = front;
        playerAnimationState = true;
        loadAllSpritesIntoData();
      

    }
   
    void update(PlayerDirection* userInput, bool* spaceBarPress) override {
       
        if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastSpriteUpdate).count()) > spriteFrequency) {
          
            playerAnimationState = !playerAnimationState;
           
            playerDirection = *userInput;

            data = loadSpriteBasedOnState();
        }

        this->draw();
    }
    void checkCollisions() override {

    }
    void draw() override{
      
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        //glTexSubImage2D(GL_TEXTURE0, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // normalized aspect ratio
        float nAR = xRes / yRes;
        float size = 0.2f;
        glm::mat4 mv = glm::mat4(
            -1 * size, 0, 0, x,
            0, nAR * -1 * size, 0, y,
            0, 0, 1, 0,
            0, 0, 0, 1);


        GLfloat matrix = glGetUniformLocation(programID, "mv");
        glUniformMatrix4fv(matrix, 1, GL_FALSE, &mv[0][0]);

    
        // Use our shader
        glUseProgram(programID);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureSamplerID, 0);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, vertexbuffer_size); // 3 indices starting at 0 -> 1 triangle

    }
    unsigned char* loadSpriteBasedOnState() {

        if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastSpriteUpdate).count()) < spriteFrequency) {
            return data;
        }

        lastSpriteUpdate = std::chrono::steady_clock::now();

        unsigned char* img;
        img = sprites[8];

        switch (playerAnimationState)
        {
        case true:
            switch (playerDirection)
            {
            case front:
                img = sprites[0];
                break;
            case back:
                img = sprites[2];
                break;
            case left:
                img = sprites[4];
                break;
            case right:
                img = sprites[6];
                break;
            case none:
                break;
            default:
                break;
            }
            break;
        case false:
            switch (playerDirection)
            {
            case front:
                img = sprites[1];
                break;
            case back:
                img = sprites[3];
                break;
            case left:
                img = sprites[5];
                break;
            case right:
                img = sprites[7];
                break;
            case none:
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        return img;
    }
    void loadAllSpritesIntoData() override {
        sprites.push_back( stbi_load("../sprites/front_1.png", &width, &height, &nrChannels, 4));
        sprites.push_back( stbi_load("../sprites/front_2.png", &width, &height, &nrChannels, 4));
        sprites.push_back( stbi_load("../sprites/back_1.png", &width, &height, &nrChannels, 4));
        sprites.push_back( stbi_load("../sprites/back_2.png", &width, &height, &nrChannels, 4));
        sprites.push_back( stbi_load("../sprites/left_1.png", &width, &height, &nrChannels, 4));
        sprites.push_back( stbi_load("../sprites/left_2.png", &width, &height, &nrChannels, 4));
        sprites.push_back( stbi_load("../sprites/right_1.png", &width, &height, &nrChannels, 4));
        sprites.push_back( stbi_load("../sprites/right_2.png", &width, &height, &nrChannels, 4));
        sprites.push_back( stbi_load("../sprites/idle.png", &width, &height, &nrChannels, 4));
    }
    bool initializeVAOs() override{
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        vertexbuffer_size = 6;

        glm::vec2 triangleVertice1 = glm::vec2(0.0f, 0.0f);
        glm::vec2 triangleVertice2 = glm::vec2(0.0f, 1.0f);
        glm::vec2 triangleVertice3 = glm::vec2(1.0f, 1.0f);

        glm::vec2 secTriangleVertice1 = glm::vec2(0.0f, 0.0f);
        glm::vec2 secTriangleVertice2 = glm::vec2(1.0f, 1.0f);
        glm::vec2 secTriangleVertice3 = glm::vec2(1.0f, 0.0f);

        static GLfloat g_vertex_buffer_data[18];
        g_vertex_buffer_data[0] = triangleVertice1[0];
        g_vertex_buffer_data[1] = triangleVertice1[1];
        g_vertex_buffer_data[2] = 0.0f;
        g_vertex_buffer_data[3] = triangleVertice2[0];
        g_vertex_buffer_data[4] = triangleVertice2[1];
        g_vertex_buffer_data[5] = 0.0f;
        g_vertex_buffer_data[6] = triangleVertice3[0];
        g_vertex_buffer_data[7] = triangleVertice3[1];
        g_vertex_buffer_data[8] = 0.0f;
        g_vertex_buffer_data[9] = secTriangleVertice1[0];
        g_vertex_buffer_data[10] = secTriangleVertice1[1];
        g_vertex_buffer_data[11] = 0.0f;
        g_vertex_buffer_data[12] = secTriangleVertice2[0];
        g_vertex_buffer_data[13] = secTriangleVertice2[1];
        g_vertex_buffer_data[14] = 0.0f;
        g_vertex_buffer_data[15] = secTriangleVertice3[0];
        g_vertex_buffer_data[16] = secTriangleVertice3[1];
        g_vertex_buffer_data[17] = 0.0f;

        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);




        textureSamplerID = glGetUniformLocation(programID, "myTextureSampler");

        static const GLfloat g_uv_buffer_date[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

        glGenBuffers(1, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_date), g_uv_buffer_date, GL_STATIC_DRAW);

        return true;
    }
    bool cleanupVAOs() override{
        // Cleanup VBO
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteVertexArrays(1, &VertexArrayID);
        return true;
    }
};


class Bullet: public GameObject {

    int hitpoints;
    glm::vec2 position;
    PlayerDirection direction;
    std::chrono::steady_clock::time_point shootTime;
    float lifeTime = 1000;

public:
    Bullet(float x, float y, PlayerDirection dir, std::chrono::steady_clock::time_point shoot) {
        direction = dir;
        float ver = xRes / yRes;
        float size = 0.2f;
       
        position.x = x - 0.14f;
        position.y = y - 0.2f;
        shootTime = shoot;
        loadAllSpritesIntoData();
        data = loadSpriteBasedOnState();
    }
    void checkCollisions() override {}
    void update(PlayerDirection* userInput, bool* spaceBarPress) override {
        if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - shootTime).count()) > lifeTime) {
            isActive = false;
        }
        switch (direction)
        {
        case front:
            position.y -= 0.018f * gameSpeed;
            break;
        case back:
            position.y += 0.018f * gameSpeed;
            break;
        case left:
            position.x -= 0.018f * gameSpeed;
            break;
        case right:
            position.x += 0.018f * gameSpeed;
            break;
        case none:
            position.y -= 0.018f * gameSpeed;
            break;
        default:
            break;
        }

        bulletsPos.push_back(position);

        float size = 0.08f;
        float res = xRes / yRes;

        translation = glm::mat4(
            size, 0, 0, position.x,
            0, size * res, 0, position.y,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
        this->draw();
    }
    void draw() override {


        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        //glTexSubImage2D(GL_TEXTURE0, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


        GLfloat matrix = glGetUniformLocation(programID, "mv");
        glUniformMatrix4fv(matrix, 1, GL_FALSE, &translation[0][0]);

        // Clear the screen


        // Use our shader
        glUseProgram(programID);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureSamplerID, 0);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, vertexbuffer_size); // 3 indices starting at 0 -> 1 triangle


    }
    bool initializeVAOs() override {
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        vertexbuffer_size = 6;

        glm::vec2 triangleVertice1 = glm::vec2(0.0f, 0.0f);
        glm::vec2 triangleVertice2 = glm::vec2(0.0f, 1.0f);
        glm::vec2 triangleVertice3 = glm::vec2(1.0f, 1.0f);

        glm::vec2 secTriangleVertice1 = glm::vec2(0.0f, 0.0f);
        glm::vec2 secTriangleVertice2 = glm::vec2(1.0f, 1.0f);
        glm::vec2 secTriangleVertice3 = glm::vec2(1.0f, 0.0f);

        GLfloat g_vertex_buffer_data[18];
        g_vertex_buffer_data[0] = triangleVertice1[0];
        g_vertex_buffer_data[1] = triangleVertice1[1];
        g_vertex_buffer_data[2] = 0.0f;
        g_vertex_buffer_data[3] = triangleVertice2[0];
        g_vertex_buffer_data[4] = triangleVertice2[1];
        g_vertex_buffer_data[5] = 0.0f;
        g_vertex_buffer_data[6] = triangleVertice3[0];
        g_vertex_buffer_data[7] = triangleVertice3[1];
        g_vertex_buffer_data[8] = 0.0f;
        g_vertex_buffer_data[9] = secTriangleVertice1[0];
        g_vertex_buffer_data[10] = secTriangleVertice1[1];
        g_vertex_buffer_data[11] = 0.0f;
        g_vertex_buffer_data[12] = secTriangleVertice2[0];
        g_vertex_buffer_data[13] = secTriangleVertice2[1];
        g_vertex_buffer_data[14] = 0.0f;
        g_vertex_buffer_data[15] = secTriangleVertice3[0];
        g_vertex_buffer_data[16] = secTriangleVertice3[1];
        g_vertex_buffer_data[17] = 0.0f;

        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);




        textureSamplerID = glGetUniformLocation(programID, "myTextureSampler");

        static const GLfloat g_uv_buffer_date[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

        glGenBuffers(1, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_date), g_uv_buffer_date, GL_STATIC_DRAW);

        return true;
    }
    bool cleanupVAOs() override {
        // Cleanup VBO
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteVertexArrays(1, &VertexArrayID);
        return true;
    }
    unsigned char* loadSpriteBasedOnState() override {
        return sprites[0];
    }
    void loadAllSpritesIntoData() override {
        sprites.push_back(stbi_load("../sprites/bullet.png", &width, &height, &nrChannels, 4));

    }
};


class Enemy : public GameObject {
    int hitpoints;
    glm::vec2 position;
public:
    Enemy(int hp, int x, int y) {
        hitpoints = hp;
        float size = 0.22f;
        float ver = xRes / yRes;


        translation = glm::mat4(
            -1 * size, 0, 0, x,
            0, -1 * size * ver, 0, y,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
        speed = 0.01f;
        isActive = true;
        radius = 0.2f;
        loadAllSpritesIntoData();
        data = loadSpriteBasedOnState();
    }

    void checkPlayerCollision() {
        float ver = xRes / yRes;
        float dx = translation[0][3] - x;
        float dy = translation[1][3] - y;


        float dist = sqrt(dx * dx + dy * dy);
        float mindistance = radius;
        if (dist < mindistance) {

            std::cout << "Collision detected!" << std::endl;
            playerHit = true;
        }
    }

    void checkCollisions() override {
        for (int i = 0; i < bulletsPos.size(); i++)
        {
            float ver = xRes / yRes;
            float dx = translation[0][3] - bulletsPos[i].x;
            float dy = translation[1][3] - bulletsPos[i].y - 0.2f;


            float dist = sqrt(dx * dx + dy * dy);
            float mindistance = radius;
            if (dist < mindistance) {

                std::cout << "bullet" << std::endl;
                isActive = false;
            }
        }
       
    }

    void update(PlayerDirection* userInput, bool* spaceBarPress) override {

        checkPlayerCollision();
       

        if (x > translation[0][3])
        {
            translation[0][3] += 0.001f * gameSpeed;
        }
        else
        {
            translation[0][3] -= 0.001f * gameSpeed;
        }

        if (y > translation[1][3])
        {
            translation[1][3] += 0.001f * gameSpeed;
        }
        else
        {
            translation[1][3] -= 0.001f * gameSpeed;
        }
        this->draw();
    }
    void draw() override {


        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        //glTexSubImage2D(GL_TEXTURE0, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);





        GLfloat matrix = glGetUniformLocation(programID, "mv");
        glUniformMatrix4fv(matrix, 1, GL_FALSE, &translation[0][0]);

        // Clear the screen


        // Use our shader
        glUseProgram(programID);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureSamplerID, 0);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, vertexbuffer_size); // 3 indices starting at 0 -> 1 triangle


    }
    bool initializeVAOs() override {
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        vertexbuffer_size = 6;

        glm::vec2 triangleVertice1 = glm::vec2(0.0f, 0.0f);
        glm::vec2 triangleVertice2 = glm::vec2(0.0f, 1.0f);
        glm::vec2 triangleVertice3 = glm::vec2(1.0f, 1.0f);

        glm::vec2 secTriangleVertice1 = glm::vec2(0.0f, 0.0f);
        glm::vec2 secTriangleVertice2 = glm::vec2(1.0f, 1.0f);
        glm::vec2 secTriangleVertice3 = glm::vec2(1.0f, 0.0f);

        GLfloat g_vertex_buffer_data[18];
        g_vertex_buffer_data[0] = triangleVertice1[0];
        g_vertex_buffer_data[1] = triangleVertice1[1];
        g_vertex_buffer_data[2] = 0.0f;
        g_vertex_buffer_data[3] = triangleVertice2[0];
        g_vertex_buffer_data[4] = triangleVertice2[1];
        g_vertex_buffer_data[5] = 0.0f;
        g_vertex_buffer_data[6] = triangleVertice3[0];
        g_vertex_buffer_data[7] = triangleVertice3[1];
        g_vertex_buffer_data[8] = 0.0f;
        g_vertex_buffer_data[9] = secTriangleVertice1[0];
        g_vertex_buffer_data[10] = secTriangleVertice1[1];
        g_vertex_buffer_data[11] = 0.0f;
        g_vertex_buffer_data[12] = secTriangleVertice2[0];
        g_vertex_buffer_data[13] = secTriangleVertice2[1];
        g_vertex_buffer_data[14] = 0.0f;
        g_vertex_buffer_data[15] = secTriangleVertice3[0];
        g_vertex_buffer_data[16] = secTriangleVertice3[1];
        g_vertex_buffer_data[17] = 0.0f;

        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);




        textureSamplerID = glGetUniformLocation(programID, "myTextureSampler");

        static const GLfloat g_uv_buffer_date[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

        glGenBuffers(1, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_date), g_uv_buffer_date, GL_STATIC_DRAW);

        return true;
    }
    bool cleanupVAOs() override {
        // Cleanup VBO
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteVertexArrays(1, &VertexArrayID);
        return true;
    }
    unsigned char* loadSpriteBasedOnState() override {
        return sprites[0];
    }
    void loadAllSpritesIntoData() override {
        sprites.push_back(stbi_load("../sprites/ghost.png", &width, &height, &nrChannels, 4));

    }

};

// public variables
std::chrono::steady_clock::time_point lastUpdate;
std::chrono::steady_clock::time_point lastShoot;
std::chrono::steady_clock::time_point lastGhostSpawn;
int cooldown = 2000;

std::vector<std::shared_ptr<GameObject>> gameObjects;

PlayerDirection userInput = none;

int main( void )
{

   
  //Initialize window
  bool windowInitialized = initializeWindow();
  if (!windowInitialized) return -1;

  glm::mat4 trans = glm::mat4(
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
      );

  std::shared_ptr<Player> p =  std::make_shared<Player>(10, trans);
   gameObjects.push_back(p);
 

 
  for (int i = 0; i < gameObjects.size(); i ++)
  {
      gameObjects[i].get()->initializeVAOs();
  }

  // Create and compile our GLSL program from the shaders
  programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

  lastUpdate = std::chrono::steady_clock::now();
 
	//start animation loop until escape key is pressed
	do{
        if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastUpdate).count()) > 33) {
            lastUpdate = std::chrono::steady_clock::now();
            updateAnimationLoop();

            
        }
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 && !playerHit);
    
	
  //Cleanup and close window
    for (int i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i].get()->cleanupVAOs();
    }
    glDeleteProgram(programID);
	
    closeWindow();
  
	return 0;
}

void updateAnimationLoop()
{
    if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastGhostSpawn).count()) > cooldown) {
        lastGhostSpawn = std::chrono::steady_clock::now();

        std::shared_ptr<Enemy> e;

        std::srand(std::time(nullptr));
        int rand = std::rand() %4 +1;
        switch (rand)
        {
        case 1:
            e  = std::make_shared<Enemy>(10, -1, -1);
            break;
        case 2:
            e = std::make_shared<Enemy>(10, -1, 1);
            break;
        case 3:
            e = std::make_shared<Enemy>(10, 1, 1);
            break;
        case 4:
            e = std::make_shared<Enemy>(10, 1, -1);
            break;
        default:
            break;
        }

        e.get()->initializeVAOs();

        gameObjects.push_back(e);
        test = false;

        if (cooldown > 200)
            cooldown -= 20;
    }

    bool shooting = false;

    userInput = none;

    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        shooting = true;
    }

    if (glfwGetKey(window, GLFW_KEY_W)) {
        y += 0.005f * gameSpeed;
        userInput = back;
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        userInput = front;
        y -= 0.005f * gameSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        userInput = left;
        x -= 0.005f * gameSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        userInput = right;
        x += 0.005f * gameSpeed;
    }

    bulletsPos.clear();

    glClear(GL_COLOR_BUFFER_BIT);
    std::vector<std::shared_ptr<GameObject>> newObjs;
    for (int i = 0; i < gameObjects.size(); i++) {
        
        if (gameObjects[i].get()->isActive) {
            gameObjects[i].get()->update(&userInput, &shooting);
            newObjs.push_back(gameObjects[i]);
        }
    }

    gameObjects.clear();
    gameObjects = newObjs;

    
     
    if (shooting)
    {
        if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastShoot).count()) > 500) {
            lastShoot = std::chrono::steady_clock::now();
            std::cout << x << ","<< y << "\n";


            std::shared_ptr<Bullet> b = std::make_shared<Bullet>(Bullet(x, y, userInput, lastShoot));
            b.get()->initializeVAOs();
            gameObjects.push_back(b);
          
        }
    }

    
    for (int i = 0; i < gameObjects.size(); i++) {
        gameObjects[i].get()->checkCollisions();
    
    }

  

   glDisableVertexAttribArray(0);
  // Swap buffers
  glfwSwapBuffers(window);
  glfwPollEvents();
  
}

bool initializeWindow()
{
  // Initialise GLFW
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return false;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(xRes  , yRes, "Ghost Shooter", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
    getchar();
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return false;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  return true;
}


bool closeWindow()
{
  glfwTerminate();
  return true;
}

