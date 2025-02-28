#include "scenebasic_uniform.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"


SceneBasic_Uniform::SceneBasic_Uniform() :
      tPrev(0), angle(90.0f), rotSpeed(0.0f), sky(100.0f), time(0.01f), plane(50.0f,50.0f,50.0f, 50.0f) {
      ogre = ObjMesh::load("media/rocknew.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    view = glm::lookAt(keyboardController.getCameraPosition(),
        keyboardController.getCameraPosition() + keyboardController.getCameraDirection(),
        glm::vec3(0.0f, 1.0f, 0.0f));

    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, SceneBasic_Uniform::mouseCallback);

    compile();
    glEnable(GL_DEPTH_TEST);
    projection = mat4(1.0f);

    modelProg.use();
    modelProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f)); // Gray fog color
    modelProg.setUniform("Fog.MinDist", 1.0f); // Fog starts at 10 units
    modelProg.setUniform("Fog.MaxDist", 20.0f); // Fog fully covers at 50 units

    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/pisa-hdr/tex");
    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    // Load textures
    GLuint mossTexture = Texture::loadTexture("media/texture/asteroidcolor.png");
    GLuint brickTexture = Texture::loadTexture("media/texture/asteroidnorm.png");

 // Bind textures to texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mossTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, brickTexture);

    cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);  // Start slightly above the ground
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);  // Looking forward
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

}

void SceneBasic_Uniform::compile()
{
	try {
		
        skyProg.compileShader("shader/skybox.vert");
        skyProg.compileShader("shader/skybox.frag");
        modelProg.compileShader("shader/model.vert");
        modelProg.compileShader("shader/model.frag");
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        modelProg.link();
        skyProg.link();
        prog.link();
        
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update(float t)
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;

    angle += rotSpeed * deltaT;
    if (angle > glm::two_pi<float>()) {
        angle -= glm::two_pi<float>();
    }
    // Update the time variable
    time += deltaT;


    float deltaTime = 0.01f;
    keyboardController.processInput(glfwGetCurrentContext(), deltaTime);

    view = glm::lookAt(keyboardController.getCameraPosition(),
        keyboardController.getCameraPosition() + keyboardController.getCameraDirection(),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos = vec3(7.0f*cos(angle), 2.0f, 7.0f*sin(angle));
    skyProg.use();
    model = mat4(1.0f);
    setMatrices(skyProg);
    sky.render();

    modelProg.use();
    modelProg.setUniform("IsPlane", true); // Indicate that this is the plane
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -5.0f, 0.0f)); // Position the plane underneath the model
    setMatrices(modelProg);
    plane.render();

    modelProg.use();
    modelProg.setUniform("IsPlane", false); // Indicate that this is not the plane
    modelProg.setUniform("textureMoss", 0); // Texture unit 0
    modelProg.setUniform("textureBrick", 1); // Texture unit 1

    #define MAX_LIGHTS 3
    vec3 lightPositions[MAX_LIGHTS] = {
        vec3(3.0f, 7.0f, 3.0f),  // Slightly to the right
        vec3(-3.0f, 7.0f, -3.0f), // Slightly to the left
        vec3(0.0f, 7.0f, 0.0f)   // Centered above
    };

    vec3 lightDirections[MAX_LIGHTS] = {
        vec3(0.0f, -1.0f, 0.0f), // Straight down
        vec3(0.0f, -1.0f, 0.0f), // Straight down
        vec3(0.0f, -1.0f, 0.0f)  // Straight down
    };


    // Set light positions and directions
    for (int i = 0; i < MAX_LIGHTS; i++) {
        std::string lightName = "Lights[" + std::to_string(i) + "]";
        modelProg.setUniform((lightName + ".Position").c_str(), vec4(lightPositions[i], 1.0f));
        modelProg.setUniform((lightName + ".La").c_str(), vec3(0.1f)); // Ambient light intensity
        modelProg.setUniform((lightName + ".L").c_str(), vec3(1.0f)); // Diffuse and specular light intensity
        modelProg.setUniform((lightName + ".Direction").c_str(), lightDirections[i]);
        modelProg.setUniform((lightName + ".Cutoff").c_str(), glm::radians(50.0f)); // Spotlight cutoff angle
    }

    // Set material uniforms
    modelProg.setUniform("Material.Kd", vec3(0.8f)); // Diffuse reflectivity
    modelProg.setUniform("Material.Ka", vec3(0.2f)); // Ambient reflectivity
    modelProg.setUniform("Material.Ks", vec3(0.5f)); // Specular reflectivity
    modelProg.setUniform("Material.Shininess", 32.0f); // Specular shininess

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -3.0f, 0.0f)); // Example position
    model = glm::scale(model, vec3(2.0f)); // Example scale
    // Add rotation: Rotate around Y-axis
    float rotationAngle = time * glm::radians(45.0f); // 45 degrees per second
    model = glm::rotate(model, rotationAngle, vec3(1.0f, 1.0f, 0.0f)); // Diagonal rotation


    setMatrices(modelProg);
    ogre->render();
    //plane.render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);

    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    SceneBasic_Uniform* scene = static_cast<SceneBasic_Uniform*>(glfwGetWindowUserPointer(window));
    if (scene) {
        scene->keyboardController.processMouseMovement(xpos, ypos);
    }
}

void SceneBasic_Uniform::setMatrices(GLSLProgram &p)
{
    mat4 mv = view*model;

    p.setUniform("ModelMatrix", model);
    p.setUniform("ViewMatrix", view);
    p.setUniform("ProjectionMatrix", projection);
    //prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[01]), vec3(mv[1]), vec3(mv[2])));
    p.setUniform("MVP",projection* mv);
}

