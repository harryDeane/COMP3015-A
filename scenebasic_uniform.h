#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/cube.h"

#include "helper/skybox.h"

class SceneBasic_Uniform : public Scene
{
private:

   // Plane plane;
   Teapot teapot;
   // Cube cube;
    SkyBox sky;
   //std::unique_ptr<ObjMesh> ogre;
   float rotSpeed;
    float tPrev;
    float angle;
   // GLuint vaoHandle; 
    GLSLProgram prog, skyProg;

   // glm::mat4 rotationMatrix;

    //Torus torus;
    void setMatrices(GLSLProgram &p);

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
