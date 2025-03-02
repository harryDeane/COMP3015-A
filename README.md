# 3D Rendering Prototype

This project is a 3D rendering prototype built using OpenGL and GLSL. It features dynamic lighting, texture manipulation, a Blinn-Phong shading model, and fog effects. Below, you'll find details about the development environment, how the code works, and how to navigate the project.

---

## Development Environment
- **Operating System**: Windows 10
- **IDE**: Visual Studio 2022
- **Libraries Used**:
  - OpenGL (via GLFW and GLEW)
  - GLM (OpenGL Mathematics)
  - stb_image (for texture loading)

---

## How Does It Work?
This prototype renders a 3D scene with the following features:
1. **Dynamic Lighting**: A light source with spotlight effects.
2. **Texture Manipulation**: Uses color textures and normal maps for detailed surface rendering and a mix texture on the plane.
3. **Blinn-Phong Shading**: Combines ambient, diffuse, and specular lighting for realistic material interactions.
4. **Fog Effect**: Adds depth by blending fragment colors with a fog color based on distance.
5. **Skybox**: A reflective skybox creates a seamless environment.
6. **Keyboard Controls**: Keyboard controls to navigate the scene.

The scene includes:
- A rotating 3D model (`rocknew.obj`) with detailed textures.
- A plane that reacts differently to lighting and textures.
- A skybox for environmental immersion.

---

## Code Structure
Here’s how the code fits together:

### Key Files
1. **`scenebasic_uniform.cpp`**:
   - Contains the main rendering logic.
   - Handles scene initialization, updates, and rendering.
   - Manages shader compilation and texture loading.

2. **`model.vert`** (Vertex Shader):
   - Transforms vertex positions into world space.
   - Calculates light and view directions for each fragment.

3. **`model.frag`** (Fragment Shader):
   - Implements the Blinn-Phong shading model.
   - Samples textures and applies lighting calculations.
   - Computes the fog effect.

4. **`skybox.vert` and `skybox.frag`**:
   - Renders the skybox using a HDR cube map texture.

5. **`helper/glutils.h`**:
   - Provides utility functions for OpenGL operations.

6. **`helper/texture.h`**:
   - Handles texture loading using `stb_image`.
     
6. **`KeyboardController.cpp`**:
   - Contains the main keyboard controls functions

### How to Navigate the Code
- **Entry Point**: Start with `scenebasic_uniform.cpp`. This file initializes the scene, compiles shaders, and sets up the main rendering loop.
- **Shaders**: The vertex and fragment shaders (`model.vert` and `model.frag`) contain the core rendering logic. Review these to understand how lighting, textures, and fog are implemented.
- **Textures**: Textures are loaded in `scenebasic_uniform.cpp` using the `Texture::loadTexture` and `Texture::loadHdrCubeMap` functions.
- **Lighting**: Light positions and properties are defined in `scenebasic_uniform.cpp` and passed to the shader as uniforms.

---

## How to Run the Code
1. **Prerequisites**:
   - Install Visual Studio 2022.
   - Set up OpenGL, GLFW, and GLEW.
   - Download the required textures and models (located in the `media` folder).

2. **Steps**:
   - Clone this repository.
   - Open the project in Visual Studio.
   - Build and run the project.

---

## Additional Notes
- **Texture Blending**: The plane uses a different texture blending approach compared to the 3D model. This is controlled by the `IsPlane` uniform in the fragment shader.
- **Fog Effect**: The fog effect is calculated based on the distance from the camera to the fragment. Adjust `Fog.MinDist` and `Fog.MaxDist` in `scenebasic_uniform.cpp` to tweak the fog intensity.
- **Dynamic Lighting**: The three light sources are positioned above the scene and cast light downward. Their properties (position, direction, and cutoff angle) can be modified in `scenebasic_uniform.cpp`.

---

## YouTube Video
For a detailed walkthrough of the project, including the development process, shader code review, and challenges faced, check out the unlisted YouTube video:  
[**Watch the Video**](https://youtu.be/zoRkNCmpIPU)
