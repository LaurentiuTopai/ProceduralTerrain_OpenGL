//
//  main.cpp
//  Check Max Supported OpenGL version (on non-apple devices)
//
//  Created by CGIS on 25/09/2023.
//  Copyright 2023 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TerrainManager.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "LoadTexture.hpp"
#include "stb_image.h"
#include "Tree.hpp"
const int GL_WINDOW_WIDTH = 1200;
const int GL_WINDOW_HEIGHT = 800;

GLFWwindow* glWindow = NULL;


gps::Shader shader;
gps::Shader waterShader;

TerrainManager tm;
Camera my_Camera(glm::vec3(100, 50, 100), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
const int divisions = 100;   
const float width = 300.0f;  
const int gridSize = 10;      

float WaterLevel = 0.25f;


std::vector<GLfloat> waterVerticies{
    -30000.0f, WaterLevel, -30000.0f,
     3000.0f, WaterLevel, -3000.0f,
     3000.0f, WaterLevel,  3000.0f,
    -3000.0f, WaterLevel,  3000.0f
};
std::vector<GLuint> waterIndices{
	0, 1, 2,
	2, 3, 0
};
glm::vec3 v0(0, 0, 0);
glm::vec3 v1(width, 0, 0);
glm::vec3 v2(width, 0, -width);
glm::vec3 v3(0, 0, -width);
GLuint waterVAO, waterVBO, waterEBO;
glm::mat4 view, projection;
glm::vec4 clearColor{ 0.5f, 0.7f, 1.0f, 1.0f };
gps::Model3D treeModel;

bool initOpenGLWindow()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }
    
    glWindow = glfwCreateWindow(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT, "Hello Window", NULL, NULL);
    if (!glWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(glWindow);

#if !defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    return true;
}

void setupTerrainBuffers() {

    int half = gridSize / 2;
    for (int x = -half; x <= half; x++) {
        for (int z = -half; z <= half; z++) {
            glm::vec3 origin = glm::vec3(x * width, 0, z * width); // f?r? minus
            tm.chunks.push_back(tm.generateChunk(origin, divisions, width));
        }
    }
}
void cleanup() {
 
    glfwDestroyWindow(glWindow);
    glfwTerminate();
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = GL_WINDOW_WIDTH / 2.0f;
    static float lastY = GL_WINDOW_HEIGHT / 2.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;


    my_Camera.ProcessMouseMovement(xoffset, yoffset);
}
void renderScene() {
    //TODO drawing code
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.useShaderProgram();
    view = my_Camera.GetViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), GL_WINDOW_WIDTH / (float)GL_WINDOW_HEIGHT, 0.1f, 1000.0f);
    
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    shader.setVec3("cameraPos", my_Camera.Position);
    shader.setVec3("lightDir", glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)));
    shader.setVec3("baseColor", glm::vec3(0.7f, 0.7f, 0.7f));
    shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    for (auto& chunk : tm.chunks) { 
        chunk.vao.Bind();
        glm::mat4 model = glm::mat4(1.0f); 
    shader.setMat4("model", model); 
    glDrawElements(GL_TRIANGLES, chunk.indexCount, GL_UNSIGNED_INT, 0); 
    chunk.vao.UnBind(); }
    glm::mat4 model = glm::mat4(1.0f);
	waterShader.useShaderProgram();
	waterShader.setMat4("view", view);
    waterShader.setMat4("model", model);
    waterShader.setMat4("projection", projection);
    waterShader.setVec3("waterColor", glm::vec3(0.0f, 0.3f, 0.8f));
    glBindVertexArray(waterVAO);
    glDrawElements(GL_TRIANGLES, waterIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

int main(int argc, const char * argv[]) {

    if (!initOpenGLWindow()) {

        return 1;
    }

    /*
    * treeModel.LoadModel("Assets/Modele3D/Trees_Bun/jacaranda_tree_4k.obj");
	float height = tm.getHeightAt(10, 10);
    Tree oneTree(glm::vec3(10, height, 10), &treeModel);

    */
   Material grass, mud, rock,pureGrass;

    shader.loadShader("Terrain.vert", "Terrain.frag");
    
    waterShader.loadShader("Water.vert", "Water.frag");

	pureGrass.albedoID = Texture::ReadTextureFromFile("Assets/Texture_Green_Grass/grass_albedo.jpg");
    grass.albedoID = Texture::ReadTextureFromFile("Assets/Texture_Sand_Grass/sand_rocks_albedo.jpg");
    mud.albedoID = Texture::ReadTextureFromFile("Assets/Texture_Mud/mud_albedo.jpg");
    rock.albedoID = Texture::ReadTextureFromFile("Assets/Texture_Rocks/rock_albedo.jpg");
    
    pureGrass.roughnessID = Texture::ReadTextureFromFileRoughness("Assets/Texture_Green_Grass/grass_roughness.png");
    grass.roughnessID = Texture::ReadTextureFromFileRoughness("Assets/Texture_Sand_Grass/sand_rocks_roughness.png");
	mud.roughnessID = Texture::ReadTextureFromFileRoughness("Assets/Texture_Mud/mud_roughness.png");
	rock.roughnessID = Texture::ReadTextureFromFileRoughness("Assets/Texture_Rocks/rock_roughness.jpg");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.useShaderProgram();
    shader.setInt("albedoGrass", 0);
    shader.setInt("albedoMud", 1);
    shader.setInt("albedoRock", 2);
    shader.setInt("albedoPureGrass", 3);

    shader.setInt("roughnessGrass",4);
	shader.setInt("roughnessMud",5);
	shader.setInt("roughnessRock", 6);
	shader.setInt("roughnessPureGrass", 7);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grass.albedoID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mud.albedoID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, rock.albedoID);
	glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, pureGrass.albedoID);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, grass.roughnessID);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mud.roughnessID);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, rock.roughnessID);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, pureGrass.roughnessID);
    glEnable(GL_DEPTH_TEST);

    setupTerrainBuffers();
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
    glGenBuffers(1, &waterEBO);

    glBindVertexArray(waterVAO);

    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, waterVerticies.size() * sizeof(GLfloat), waterVerticies.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterIndices.size() * sizeof(GLuint), waterIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glm::vec3 oldPos = my_Camera.Position;
    glm::vec3 desiredPos = oldPos;
    float deltaTime = 0.0f;
    float lastTime = 0.0f;
    const float cameraHeightOffset = 2.0f;

    float gravity = -9.81f;
    float velocityY = 0;
	float jumpForce = 5.0f;
	bool onGround = false;
    bool noclip = false;
    while (!glfwWindowShouldClose(glWindow)) {
        float CurrentTime = glfwGetTime();
        deltaTime = CurrentTime - lastTime;
        lastTime = CurrentTime;
        if (glfwGetKey(glWindow, GLFW_KEY_W) == GLFW_PRESS)
            desiredPos += my_Camera.Front * my_Camera.MovementSpeed * deltaTime;
        if (glfwGetKey(glWindow, GLFW_KEY_S) == GLFW_PRESS)
            desiredPos -= my_Camera.Front * my_Camera.MovementSpeed * deltaTime;
        if (glfwGetKey(glWindow, GLFW_KEY_A) == GLFW_PRESS)
            desiredPos -= my_Camera.Right * my_Camera.MovementSpeed * deltaTime;
        if (glfwGetKey(glWindow, GLFW_KEY_D) == GLFW_PRESS)
            desiredPos += my_Camera.Right * my_Camera.MovementSpeed * deltaTime;
        if (glfwGetKey(glWindow, GLFW_KEY_M) == GLFW_PRESS)
            cleanup();
        if (glfwGetKey(glWindow, GLFW_KEY_N) == GLFW_PRESS) {
            noclip = !noclip;
        }

        if (!noclip) {
            float hCenter = tm.getHeightAt(desiredPos.x, desiredPos.z);
            float hLeft = tm.getHeightAt(desiredPos.x - 0.5f, desiredPos.z);
            float hRight = tm.getHeightAt(desiredPos.x + 0.5f, desiredPos.z);
            float hFront = tm.getHeightAt(desiredPos.x, desiredPos.z + 0.5f);
            float hBack = tm.getHeightAt(desiredPos.x, desiredPos.z - 0.5f);

            float terrainHeight = std::max({ hCenter, hLeft, hRight, hFront, hBack });
            if (onGround && glfwGetKey(glWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
                velocityY = jumpForce;
                onGround = false;
            }
            velocityY += gravity * deltaTime;
            desiredPos.y += velocityY * deltaTime;
            if (desiredPos.y < terrainHeight + cameraHeightOffset) {
                desiredPos.y = terrainHeight + cameraHeightOffset;
                velocityY = 0;
                onGround = true;
            }
            else {
                onGround = false;
            }
        }
        else {
            if (glfwGetKey(glWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
                desiredPos.y += my_Camera.MovementSpeed * deltaTime;
            if (glfwGetKey(glWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                desiredPos.y -= my_Camera.MovementSpeed * deltaTime; 
        }
        

        my_Camera.Position = desiredPos;
        renderScene();
        glfwSetCursorPosCallback(glWindow, mouse_callback);
        glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapBuffers(glWindow);
        glfwPollEvents();
    }
    
    cleanup();
    return 0;
}
