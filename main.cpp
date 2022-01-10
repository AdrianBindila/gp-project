#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

#define WIDTH 1024
#define HEIGHT 768
// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, -10.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
GLfloat delta = 0;
GLfloat lastFrame = 0;
GLfloat cameraSpeed = 10.0f;
GLfloat cameraRotation = 100.0f;

//mouse
float prevX = WIDTH / 2.;
float prevY = HEIGHT / 2.;
bool firstMouse = true;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
GLfloat anglePitch;
GLfloat angleYaw;

// shaders
gps::Shader myBasicShader;


//skybox
gps::SkyBox skyBox;
gps::Shader skyBoxShader;

GLenum glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow *window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    myWindow.setWindowDimensions(WindowDimensions{width, height});
    myBasicShader.useShaderProgram();

    projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));

    skyBoxShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));

    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        prevX = xpos;
        prevY = ypos;
        firstMouse = false;
    }
    float deltaX = xpos - prevX;
    float deltaY = prevY - ypos;
    prevX = xpos;
    prevY = ypos;

    float sensitivity = 2.5f;
    deltaX *= sensitivity;
    deltaY *= sensitivity;

    myCamera.rotate(glm::radians(deltaY), glm::radians(deltaX));
}

void processMovement() {
    /*Controls:
     * WASD - forward, left, backwards, right
     * Space - up
     * LCtrl - down
     * LShift - turbo
     * mouse - look
     * arrows - look
     * Q,E - rotate teapot
     * R - enable/disable wireframe
     * T - Wireframe
     * Y - Polygonal shading
     * U - Unlock mouse
    */
    //camera movement
    float deltaSpeed = cameraSpeed * delta;
    if (pressedKeys[GLFW_KEY_LEFT_SHIFT])//activate turbo
        deltaSpeed *= 4;

    if (pressedKeys[GLFW_KEY_SPACE]) {
        myCamera.move(gps::MOVE_UPWARD, deltaSpeed);
    }

    if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
        myCamera.move(gps::MOVE_DOWNWARD, deltaSpeed);
    }

    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, deltaSpeed);
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, deltaSpeed);
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, deltaSpeed);
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, deltaSpeed);
    }

    if (pressedKeys[GLFW_KEY_LEFT]) {
        myCamera.rotate(0, -(cameraRotation * delta));
    }

    if (pressedKeys[GLFW_KEY_RIGHT]) {
        myCamera.rotate(0, cameraRotation * delta);
    }

    if (pressedKeys[GLFW_KEY_UP]) {
        myCamera.rotate(cameraRotation * delta, 0);
    }

    if (pressedKeys[GLFW_KEY_DOWN]) {
        myCamera.rotate(-(cameraRotation * delta), 0);
    }

    //update view matrix
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    //object movement
    //teapot - rotate
    if (pressedKeys[GLFW_KEY_Q]) {
        angleYaw -= 1.0f;
    }
    if (pressedKeys[GLFW_KEY_E]) {
        angleYaw += 1.0f;
    }
    if(pressedKeys[GLFW_KEY_Z]){
        anglePitch-=1.0f;
    }
    if(pressedKeys[GLFW_KEY_C]){
        anglePitch+=1.0f;
    }
    // update model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angleYaw), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(anglePitch), glm::vec3(0, 0, 1));
    // update normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    //others
    if (pressedKeys[GLFW_KEY_R]) {//reset
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glShadeModel(GL_SMOOTH);
        glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (pressedKeys[GLFW_KEY_T]) {//wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_Y]) {//polygonal
        glShadeModel(GL_FLAT);
    }
    if (pressedKeys[GLFW_KEY_U]) {//unlock mouse
        glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void initOpenGLWindow() {
    myWindow.Create(WIDTH, HEIGHT, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initSkyBox() {
    std::vector<const GLchar *> faces;
    faces.push_back("../skybox/right.jpg");
    faces.push_back("../skybox/left.jpg");
    faces.push_back("../skybox/top.jpg");
    faces.push_back("../skybox/bottom.jpg");
    faces.push_back("../skybox/front.jpg");
    faces.push_back("../skybox/back.jpg");
    skyBox.Load(faces);
}

void initModels() {
    teapot.LoadModel("../models/others/Map_v1.obj");
}

void initShaders() {
    myBasicShader.loadShader("../shaders/basic.vert", "../shaders/basic.frag");
    skyBoxShader.loadShader("../shaders/skyboxShader.vert", "../shaders/skyboxShader.frag");
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(anglePitch), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angleYaw), glm::vec3(0.0f, 0.0f, 1.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");


    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
                                  (float) myWindow.getWindowDimensions().width /
                                  (float) myWindow.getWindowDimensions().height,
                                  0.1f, 1000.0f);
    // send projection matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //skybox
    skyBoxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render the scene

    // render the teapot
    renderTeapot(myBasicShader);
    skyBox.Draw(skyBoxShader, view, projection);
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

void processDelta() {
    //ensure movement consistency regardless of fps
    float newFrame = glfwGetTime();
    delta = newFrame - lastFrame;
    lastFrame = newFrame;
}

int main(int argc, const char *argv[]) {
    try {
        initOpenGLWindow();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    initOpenGLState();
    initModels();
    initSkyBox();
    initShaders();
    initUniforms();
    setWindowCallbacks();
    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processDelta();
        processMovement();
        renderScene();
        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());
        glCheckError();
    }
    cleanup();
    return EXIT_SUCCESS;
}
