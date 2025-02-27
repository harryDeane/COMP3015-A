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
    tPrev(0), angle(90.0f),rotSpeed(glm::pi<float>()/30.0f),sky(100.0f), time(1.0f) {
      ogre = ObjMesh::load("media/rock.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    projection = mat4(1.0f);
    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/pisa-hdr/pisa");
    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    // Load textures
    GLuint mossTexture = Texture::loadTexture("media/texture/brick1.jpg");
    GLuint brickTexture = Texture::loadTexture("media/texture/moss.png");

 // Bind textures to texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mossTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, brickTexture);

}

void SceneBasic_Uniform::compile()
{
	try {
		
        skyProg.compileShader("shader/skybox.vert");
        skyProg.compileShader("shader/skybox.frag");
        modelProg.compileShader("shader/model.vert");
        modelProg.compileShader("shader/model.frag");
        modelProg.link();
        skyProg.link();
        modelProg.use();
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
    if (angle>glm::two_pi<float>()) {
        angle -= glm::two_pi<float>();

    }
    // Update the time variable
    time += deltaT;
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos = vec3(7.0f*cos(angle), 2.0f, 7.0f*sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    skyProg.use();
    model = mat4(1.0f);
    setMatrices(skyProg);
    sky.render();
    modelProg.use();
    modelProg.setUniform("textureMoss", 0); // Texture unit 0
    modelProg.setUniform("textureBrick", 1); // Texture unit 1

    #define MAX_LIGHTS 3
    // Define multiple lights with slight movement using the time variable
    vec3 lightPositions[MAX_LIGHTS] = {
        vec3(5.0f * cos(time * 0.5f), 5.0f, 5.0f * sin(time * 0.5f)), // Light 1: Circular motion
        vec3(-5.0f, 5.0f + 2.0f * sin(time * 0.3f), -5.0f), // Light 2: Vertical oscillation
        vec3(0.0f, 10.0f, 0.0f + 3.0f * cos(time * 0.4f)) // Light 3: Horizontal oscillation
    };

    vec3 lightDirections[MAX_LIGHTS] = {
        vec3(0.0f, -1.0f, 0.0f),
        vec3(0.0f, -1.0f, 0.0f),
        vec3(0.0f, -1.0f, 0.0f)
    };

    // Set light positions and directions
    for (int i = 0; i < MAX_LIGHTS; i++) {
        std::string lightName = "Lights[" + std::to_string(i) + "]";
        modelProg.setUniform((lightName + ".Position").c_str(), vec4(lightPositions[i], 1.0f));
        modelProg.setUniform((lightName + ".La").c_str(), vec3(0.1f)); // Ambient light intensity
        modelProg.setUniform((lightName + ".L").c_str(), vec3(1.0f)); // Diffuse and specular light intensity
        modelProg.setUniform((lightName + ".Direction").c_str(), lightDirections[i]);
        modelProg.setUniform((lightName + ".Cutoff").c_str(), glm::radians(30.0f)); // Spotlight cutoff angle
    }

    // Set material uniforms
    modelProg.setUniform("Material.Kd", vec3(0.8f)); // Diffuse reflectivity
    modelProg.setUniform("Material.Ka", vec3(0.2f)); // Ambient reflectivity
    modelProg.setUniform("Material.Ks", vec3(0.5f)); // Specular reflectivity
    modelProg.setUniform("Material.Shininess", 32.0f); // Specular shininess

    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.0f, -2.0f, 0.0f)); // Example position
    model = glm::scale(model, vec3(0.1f)); // Example scale
    setMatrices(modelProg);
    ogre->render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);

    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
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