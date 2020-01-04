/*
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "resource_manager.h"
#include "shader_loader.h"
#include "model_loader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 1.0f), // Position
              glm::vec3(0.0f, 1.0f, 0.0f), // World up
              0.0f,                        // Yaw
              0.0f,                        // Pitch
              45.0f,                       // Fovy
              2.5f,                        // Movement speed
              0.1f);                       // Mouse sensitivity

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool  firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos = glm::vec3(0.2f, 0.2f, 0.2f);

int main()
{
   // Initialize GLFW
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   // Create GLFW window
   GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
   if (window == NULL)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   glfwSetCursorPosCallback(window, mouse_callback);
   glfwSetScrollCallback(window, scroll_callback);

   // Tell GLFW to capture our mouse
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   // Load all the OpenGL function pointers using GLAD
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
   }

   // Load the shaders
   ResourceManager<Shader> shaderManager;
   shaderManager.loadResource<ShaderLoader>("model", "shaders/16.2.model_shader.vs", "shaders/16.2.model_shader.fs");
   shaderManager.loadResource<ShaderLoader>("lamp", "shaders/6.2.lamp.vs", "shaders/6.2.lamp.fs");
   shaderManager.loadResource<ShaderLoader>("basic", "shaders/16.1.basic_shader.vs", "shaders/16.1.basic_shader.fs");
   auto modelShader = shaderManager.getResource("model");
   auto lampShader  = shaderManager.getResource("lamp");
   auto basicShader = shaderManager.getResource("basic");

   // Load the model
   ResourceManager<Model> modelManager;
   modelManager.loadResource<ModelLoader>("teapot", "objects/teapot/high_poly_with_mat/Teapot.obj");
   auto teapotModel = modelManager.getResource("teapot");

   // Create the lamp
   //                         Positions            Normals              Texture coords
   //                        <--------------->    <--------------->    <------->
   float lampVertices[] = { -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
                             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
                             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

                            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
                             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
                             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
                             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
                            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
                            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

                            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                            
                             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
                             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                            
                            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
                             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
                             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
                             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
                            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
                            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
                            
                            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
                             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
                             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
                             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
                            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
                            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f };

   unsigned int lampVAO, lampVBO;
   glGenVertexArrays(1, &lampVAO);
   glGenBuffers(1, &lampVBO);
   glBindVertexArray(lampVAO);
   glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

   // Create the background plane
   GLfloat basicVertices[] = { // Pos            // Col
                               0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                               1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

                               0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                               1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                               1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f };

   unsigned int basicVAO, basicVBO;
   glGenVertexArrays(1, &basicVAO);
   glGenBuffers(1, &basicVBO);
   glBindVertexArray(basicVAO);
   glBindBuffer(GL_ARRAY_BUFFER, basicVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(basicVertices), basicVertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

   // In perspective, the lower-left corner is the origin
   // In orthographic, the upper-left corner is the origin

   // Create the test plane
   GLfloat testVertices[] =  { // Pos            // Col
                               0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // Blue   - Upper left
                               1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // Green  - Lower right
                               0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Red    - Lower left

                               0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // Cyan   - Upper left
                               1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,   // Yellow - Upper right
                               1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f }; // White  - Lower right

   unsigned int testVAO, testVBO;
   glGenVertexArrays(1, &testVAO);
   glGenBuffers(1, &testVBO);
   glBindVertexArray(testVAO);
   glBindBuffer(GL_ARRAY_BUFFER, testVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(testVertices), testVertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

   // Execute the render loop
   while (!glfwWindowShouldClose(window))
   {
      // Per-frame time logic
      float currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      processInput(window);

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glm::mat4 model;
      glm::mat4 view;
      glm::mat4 projection;

      // Draw the background using an ortographic projection
      // ---------------------------------------------------

      // Disable depth testing for 2D objects
      glDisable(GL_DEPTH_TEST);

      model      = glm::scale(glm::mat4(), glm::vec3(1280.0f, 720.0f, 1.0f));
      view       = glm::mat4();
      projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

      basicShader->use();
      basicShader->setMat4("model", model);
      basicShader->setMat4("view", view);
      basicShader->setMat4("projection", projection);

      glBindVertexArray(basicVAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // Draw a quarter of the background using a perspective projection
      // ---------------------------------------------------------------

      // Enable depth testing for 3D objects
      glEnable(GL_DEPTH_TEST);

      model      = glm::translate(glm::mat4(), glm::vec3(-0.5f * 0.4f, -0.5f * 0.4f, 0.0f));
      model      = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
      view       = camera.getViewMatrix();
      projection = glm::perspective(glm::radians(camera.getFieldOfViewYInDeg()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

      basicShader->use();
      basicShader->setMat4("model", model);
      basicShader->setMat4("view", view);
      basicShader->setMat4("projection", projection);

      glBindVertexArray(testVAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // Draw the model using a perspective projection
      // ---------------------------------------------------

      // Rotate the light source
      glm::vec3 rotationAxis      = glm::vec3(0.0f, 0.0f, 1.0f);
      glm::mat4 rotMatrixForLight = glm::rotate(glm::mat4(), (float)glfwGetTime(), rotationAxis);
      glm::vec3 rotatedLightPos   = ((glm::mat3)(rotMatrixForLight)) * lightPos;

      // Model dimensions: (24.12f, 11.81f, 15.0f)
      //glm::mat4 rotMatrixForTeapot = glm::rotate(glm::mat4(), (float) glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
      model      = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
      model      = glm::scale(model, glm::vec3(0.01f));
      view       = camera.getViewMatrix();
      projection = glm::perspective(glm::radians(camera.getFieldOfViewYInDeg()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

      modelShader->use();
      modelShader->setMat4("model", model);
      modelShader->setMat4("view", view);
      modelShader->setMat4("projection", projection);

      // Camera properties
      modelShader->setVec3("viewPos", camera.getPosition());

      // Light properties
      modelShader->setVec3("pointLight.position", rotatedLightPos);
      modelShader->setVec3("pointLight.color", 1.0f, 1.0f, 1.0f);
      modelShader->setFloat("pointLight.constant", 1.0f);
      modelShader->setFloat("pointLight.linear", 0.09f);
      modelShader->setFloat("pointLight.quadratic", 0.032f);

      teapotModel->render(*modelShader);

      // Draw the lamp using an ortographic project
      // ---------------------------------------------------

      model      = glm::translate(glm::mat4(), rotatedLightPos);
      model      = glm::scale(model, glm::vec3(0.01f));
      view       = camera.getViewMatrix();
      projection = glm::perspective(glm::radians(camera.getFieldOfViewYInDeg()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

      lampShader->use();
      lampShader->setMat4("model", model);
      lampShader->setMat4("view", view);
      lampShader->setMat4("projection", projection);

      glBindVertexArray(lampVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   glDeleteVertexArrays(1, &lampVAO);
   glDeleteBuffers(1, &lampVBO);

   glfwTerminate();
   return 0;
}

void processInput(GLFWwindow* window)
{
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera.processKeyboardInput(MovementDirection::Forward, deltaTime);
   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera.processKeyboardInput(MovementDirection::Backward, deltaTime);
   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      camera.processKeyboardInput(MovementDirection::Left, deltaTime);
   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      camera.processKeyboardInput(MovementDirection::Right, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // Make sure the viewport matches the new window dimensions
   glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
   if (firstMouse)
   {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
   }

   float xoffset = xpos - lastX;
   float yoffset = lastY - ypos; // Reversed since Y-coordinates go from the bottom to the top

   lastX = xpos;
   lastY = ypos;

   camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
   camera.processScrollWheelMovement(yoffset);
}
*/

// Additional information:
// The process of transforming 3D coordinates to 2D pixels is managed by OpenGL's graphics pipeline
// The graphics pipeline can be divided into two large parts:
// 1) 3D coordinates -> 2D coordinates
// 2) 2D coordinates -> Colored pixels
// A 2D coordinate is a precise representation of where a point is in 2D space
// A 2D pixel is an approximation of a 2D point that is limited by the resolution of the window
// The steps that compose the graphics pipeline can be executed in parallel by running small programs on the GPU
// These small programs are called shaders
// Stages of the graphics pipeline:
// Vertex Shader -> Shape Assembly -> Geometry Shader -> Rasterization -> Fragment Shader -> Tests and Blending

// A vertex is a collection of data for a 3D coordinate (e.g. position, color, normal...)
// A vertex's data is represented using vertex attributes

// In order for OpenGL to know what to make of a set of vertices, you must hint what kind of render type you want to form with the data
// Do we want the data to be rendered as a collection of points, a collection of triangles or a line?
// The hints that tell OpenGL what to make of a set of vertices are called primitives.
// Examples: GL_POINTS, GL_TRIANGLES and GL_LINE_STRIP

// A fragment is all the data required by OpenGL to render a single pixel.

// Steps:
// 1) Vertex Shader: Takes as input a single vertex and transforms its position (3D -> 3D). Also allows us to do some basic processing on the vertex's attributes.
// 2) Shape Assembly: Takes as input all the vertices that form a primitive and assembles them in the primitive shape we specified.
// 3) Geometry Shader: Takes as input a collection of vertices that form a primitive and has the ability to generate other shapes by emitting new vertices to form other primitives.
// 4) Rasterization: Takes as input a primitive and maps it to its corresponding pixels on the screen, resulting in fragments for the fragment shader to use. Note that before the fragment shader runs, clipping is performed. Clipping discards all the fragments that are outside of view.
// 5) Fragment shader: Takes as input a fragment. Its main purpose is to calculate the final color of a pixel. This is where all the advanced OpenGL effects occur. The fragment shader usually contains data about the 3D scene that it can use to perform lighting calculations.
// 6) Alpha Test and Blending: Checks the depth and stencil values of a fragment to determine if it is in front or behind other objects. Additionally, it checks the alpha value and blends objects accordingly.

// We can play with the vertex, fragment and geometry shaders.

// OpenGL transforms the 3D coordinates that are in the range [-1.0, 1.0] on all 3 axes (x, y and z) to 2D pixels on the screen
// This range is called the Normalized Device Coordinates (NDC) range
// After vertex coordinates have been processed by the vertex shader, they should be in the NDC range
// Any coordinates outside of this range are clipped
// Normalized Deviced Coordinates are transformed to screen-space coordinates by the viewport transform (glViewport takes care of this)
// The resulting screen-space coordinates are transformed into fragments and given to the fragment shader

// So, how do we send vertex data to the vertex shader? Steps:
// 1) Create memory on the GPU where we can store the vertex data
// 2) Configure how OpenGL should interpret the memory
// 3) Specify how to send the data to the graphics card
// Let's break each step down:

// 1) Create memory on the GPU where we can store the vertex data
//       - We manage this memory through Vertex Buffer Objects (VBO). A VBO can store a large number of vertices in the GPU's memory.
//       - Using VBOs is advantageous because they allow us to send large batches of data to the graphics card all at once. Sending data from the CPU to the graphics card is slow, so we must try to send as much data as possible at once.

// float vertices[] = {...};           // Array containing the coordinates of 3 vertices
// unsigned int VBO;                   // Will contain unique ID (GLuint) corresponding to a buffer object
// glGenBuffers(1, &VBO);              // Generate one buffer object and store its unique ID in VBO
// glBindBuffer(GL_ARRAY_BUFFER, VBO); // To change the state of a buffer object, it must first be bound to a buffer binding target
                                       // Only a single buffer object can be bound to a given buffer binding target at a time, but different buffer objects can be bound to different buffer binding targets simultaneously
                                       // Once a buffer object has been bound, any calls made on its buffer binding target affect its state
                                       // Calling glBindBuffer with 0 unbinds the current buffer object
                                       // Common buffer binding targets are: GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER and GL_TEXTURE_BUFFER
// glBufferData(GL_ARRAY_BUFFER,       // Copy the vertex data into VBO's memory by calling glBufferData with the buffer binding target that VBO is bound to
//              sizeof(vertices),
//              vertices,
//              GL_STATIC_DRAW);       // This fourth parameter specifies how the graphics card should manage the given data. It can be set to 3 different values:
                                       // GL_STATIC_DRAW:  The data is not likely to change, or it will rarely change.
                                       // GL_DYNAMIC_DRAW: The data is likely to change often.
                                       // GL_STREAM_DRAW:  The data will change every time it is drawn.

// 2) Configure how OpenGL should interpret the memory
//       - How should OpenGL interpret the vertex data in memory?
//       - How should it connect the vertex data to the vertex shader's attributes?

// The vertex shader allows us to specify any input we want in the form of vertex attributes
// We have to tell OpenGL how to connect our input data with the vertex attributes
// In our case, the data in VBO is organized as follows:
//         |  Vertex1  |  Vertex2  |  Vertex3  |
//         | X   Y   Z | X   Y   Z | X   Y   Z |
//         |   |   |   |   |   |   |   |   |   |
// Byte:   0   4   8   12  16  20  24  28  32  36
// Offset: 0
// Pos:    ------------>
//          Stride: 12

// Each coordinate is a 32-bit (4 byte) floating point value
// Each position is composed of 3 coordinates (12 bytes)
// There is no space between each set of 3 values, that is, the values are tightly packed
// The first value is at the beginning of the buffer

// Knowing this, we can tell OpenGL how to interpret the vertex data for each attribute
// glVertexAttribPointer tells OpenGL how to connect the vertex data with a specific attribute when a drawing call is made.
// Each vertex attribute takes its data from the VBO that is bound to GL_ARRAY_BUFFER when the call to glVertexAttribPointer is made.

// glVertexAttribPointer(0,                 // Index:      Specifies which vertex attribute we want to configure.
                                            //             In the vertex shader we declared the following:
                                            //             layout (location = 0) in vec3 aPos;
                                            //             So index 0 corresponds to the position vertex attribute.
//                       3,                 // Size:       Specifies the number of elements that make up the vertex attribute.
                                            //             Since the position is a vec3, we select 3.
//                       GL_FLOAT,          // Type:       Specifies the type of the elements that make up the vertex attribute.
//                       GL_FALSE,          // Normalized: Specifies if the data should be normalized.
//                       3 * sizeof(float), // Stride:     Specifies the space between consecutive vertex attribute sets.
                                            //             Since the next set of position data is located exactly 3 times the size of a float away,
                                            //             we specify that value as the stride.
//                       (void*) 0);        // Offset:     Specifies the offset of where the position data begins in the buffer.

// glEnableVertexAttribArray(0);            // Enables a generic vertex attribute. A vertex attribute can be disabled by calling glDisableVertexAttribArray.

// Another example of how the vertex data is connected with the vertex attributes:

   //                Position    Color             TexCoords
   //                <-------->  <-------------->  <-------->
// GLfloat data[] = {1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.5f,
//                   0.0f, 1.0f, 0.2f, 0.8f, 0.0f, 0.0f, 1.0f};
   // Position
// glEnableVertexAttribArray(0);
// glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*) 0);
//
   // Color
// glEnableVertexAttribArray(1);
// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*) (2 * sizeof(GL_FLOAT)));
//
   // TexCoords
// glEnableVertexAttribArray(2);
// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*) (5 * sizeof(GL_FLOAT)));

// So far we have the following:
// - We initialized the vertex data in a buffer using a vertex buffer object.
// - We set up vertex and fragment shaders and told OpenGL how to link the vertex data to the vertex attributes of the vertex shader.

// So drawing in OpenGL looks like this:

// A) Copy our vertices array into a buffer for OpenGL to use.
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
// B) Set the vertex attribute pointers.
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
//    glEnableVertexAttribArray(0);
//
// C) Use our shader program when we want to render an object.
//
//    glUseProgram(shaderProgram);
//
// D) Draw the object.
//
//    ...

// If you have many objects, you need to do this for each one:
// - Bind the appropriate buffer object
// - Configure all the vertex attributes

// This is a long process. Is there a way to make it faster? Yes! Use Vertex Array Objects (VAO).
// A VAO can be bound just like a VBO, and once it is bound, it stores any vertex attribute calls.
// So we no longer need to configure all of the vertex attribute pointers multiple times.
// Instead, when we want to draw an object, we can simply load the VAO that was bound when we made the vertex attributes calls for said object.
// VAOs make it easy to switch between different vertex data and attribute configurations.

// A VAO stores the following:
// - Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
// - Vertex attribute configurations (these configurations are established using glVertexAttribPointer).
// - VBOs that are associated with vertex attributes (these associations are established when glVertexAttribPointer is called).

// When a VAO has been bound, any subsequent VBO, EBO, glVertexAttribPointer and glEnableVertexAttribArray calls are stored inside of it.
// So a VAO stores a vertex attribute configuration and which VBO to use.

// Creating a VAO is similar to creating a VBO:
// unsigned int VAO;
// glGenVertexArrays(1, &VAO);

// Using a VAO looks like this:
// A) Bind the VAO
//
// glBindVertexArray(VAO);
//
// B) Copy the vertices array into a buffer for OpenGL to use.
//
// glBindBuffer(GL_ARRAY_BUFFER, VBO);
// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
// C) Set the vertex attribute pointers.
//
// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
// glEnableVertexAttribArray(0); 
//
// D) Inside the render loop, bind the VAO and draw the object.
//
// glUseProgram(shaderProgram);
// glBindVertexArray(VAO);
// ...

// 3) Specify how to send the data to the graphics card

// To draw, OpenGL provides glDrawArrays, which draws primitives using:
// - The currently active shader program.
// - The previously defined vertex attribute configuration.
// - The vertex data in the currently bound VBO, or the vertex data that can be accessed indirectly through the currently bound VAO

// glUseProgram(shaderProgram);
// glBindVertexArray(VAO);
// glDrawArrays(GL_TRIANGLES, // Mode:    Specifies the kind of primitive to render. The options are:
                              //          GL_POINT, GL_LINE, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLE, GL_TRIANGLE_STRIP and GL_TRIANGLE_FAN.
//              0,            // First:   Specifies the starting index in the enabled arrays.
//              3);           // Count:   Specifies the number of vertices to draw.
                              //          The vertices are obtained directly from the currently bound VBO,
                              //          or indirectly through the currently bound VAO.

// Element Buffer Objects (EBO)
// Take a look at how we specify this rectangle:

// float vertices[] = { // First triangle
//                      0.5f,  0.5f, 0.0f,   // Top right
//                      0.5f, -0.5f, 0.0f,   // Bottom right
//                     -0.5f,  0.5f, 0.0f,   // Top left 
//                      // Second triangle
//                      0.5f, -0.5f, 0.0f,   // Bottom right
//                     -0.5f, -0.5f, 0.0f,   // Bottom left
//                     -0.5f,  0.5f, 0.0f }; // Top left

// Bottom right and top left are specified twice. Is there a way to avoid this overhead? Yes! With EBOs.
// An EBO is a buffer, just like a VBO, that stores indices that OpenGL uses to decide what vertices to draw.
// This is called indexed drawing.

// Using EBOs we can draw our rectangle like this (now we only need 4 vertices!):
//
// float vertices[] = { 0.5f,  0.5f, 0.0f,   // Top right
//                      0.5f, -0.5f, 0.0f,   // Bottom right
//                     -0.5f, -0.5f, 0.0f,   // Bottom left
//                     -0.5f,  0.5f, 0.0f }; // Top left
//
// unsigned int indices[] = { 0, 1, 3,       // First triangle
//                            1, 2, 3 };     // Second triangle
//
// unsigned int EBO;
// glGenBuffers(1, &EBO);
//
   // We use GL_ELEMENT_ARRAY_BUFFER as the buffer binding target
// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
   // We use glDrawElements instead of glDrawArrays to indicate that we want to render triangles from an index buffer
// glDrawElements(GL_TRIANGLES,    // Mode:    Specifies the mode we want to draw in.
//                6,               // Count:   Specifies the number of elements we want to draw.
//                GL_UNSIGNED_INT, // Type:    Specifies the type of the indices.
//                0);              // Indices: Specifies an offset in a buffer or a pointer to the location where the indices are stored (if an EBO is not used).

// glDrawElements takes its indices from the EBO that is currently bound to the GL_ELEMENT_ARRAY_BUFFER target.
// So we have to bind the corresponding EBO each time we want to render an object with indices.
// This is cumbersome. Is there a way to avoid it? Yes! Use VAOs.
// When a EBO is bound after a VAO has been bound, the VAO stores the EBO.
// In other words, a VAO stores the glBindBuffer calls when the target is GL_ELEMENT_ARRAY_BUFFER.
// This also means it stores the calls that unbind an EBO from the target GL_ELEMENT_ARRAY_BUFFER.
// So make sure you don't unbind the EBO before unbinding the VAO.

// The initialization code now looks like this:

// Initialization:
//
// A) Bind the VAO
//
// glBindVertexArray(VAO);
//
// B) Copy the vertices array into a vertex buffer for OpenGL to use
//
// glBindBuffer(GL_ARRAY_BUFFER, VBO);
// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
// B) Copy the indices array into an element buffer for OpenGL to use
//
// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
// D) Set the vertex attribute pointers
//
// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
// glEnableVertexAttribArray(0);
//
// ...
//
// Render loop:
//
// glUseProgram(shaderProgram);
// glBindVertexArray(VAO);
// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)
// glBindVertexArray(0);

// Shader structure:
// - Version declaration
// - List of input, output and uniform variables
// - Main function

// When we're talking about the vertex shader, each input variable is also known as a vertex attribute.
// The hardware determines the maximum number of vertex attributes we are allowed to declare.
// OpenGL guarantees there are always at least 16 4-component vertex attributes available.
// To know the maximum number of vertex attributes supported by your hardware, query GL_MAX_VERTEX_ATTRIBS:

// int numVertexAttributes;
// glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numVertexAttributes);

// GLSL basic types: int, float, double, uint and bool.
// GLSL container types: vectors and matrices.

// A vector in GLSL is a 1,2,3 or 4 component container for any of the basic types mentioned above.
// They can take the following form (n represents the number of components):
// vecn:  the default vector of n floats.
// bvecn: a vector of n booleans.
// ivecn: a vector of n integers.
// uvecn: a vector of n unsigned integers.
// dvecn: a vector of n double components.

// The components of vectors can be accessed with xyzw, rgba and stpw.
// Swizzling is also supported.

// Inputs and outputs:
// Each shader can specify inputs and outputs using the in and out keywords.
// When an output variable matches an input variable of the next shader stage, the output values are passed along.

// Note that vertex shaders and fragment shaders differ in terms of their inputs.
// A vertex shader receives its input straight from the vertex data, since it is the first stage of the rendering pipeline.
// To define how the vertex data is organized, we specify the input variables of the vertex shader with location metadata (e.g. layout (location = 0)).
// By doing this, we are allowed to configure the vertex attributes on the CPU.
// The vertex shader thus requires an extra layout specification for its inputs so that we can link them with the vertex data.
// Note that it is possible to omit the layout (location = 0) specifier and query for the attribute locations in your code using glGetAttribLocation.

// Another difference is that fragment shaders require a vec4 color output variable, since fragment shaders need to generate a final output color.
// If you do not to specify an output color in your fragment shader, OpenGL will render your object black (or white).

// If we want to send data from one shader to another, we have to declare an output in the sending shader and a similar input in the receiving shader.
// When the output of the sending shader and the input of the receiving shader have the same names and types, OpenGL links them together.
// Once this is done, it is possible to send data between the two shaders.
// This connection between input and outputs is established when we link a program object.

// Uniforms:
// Uniforms are another way to pass data from our application on the CPU to the shaders on the GPU.
// Uniforms differ from vertex attributes in two ways:
// - Uniforms are global.
//   Global, meaning that a uniform variable is unique per shader program object, and can be accessed from any shader at any stage in the shader program.
// - When you set the value of a uniform variable, it keeps its value until it is either reset or updated.

// If you declare a uniform that is not used anywhere in your GLSL code, the compiler will silently
// remove the variable from the compiled versiom. This can lead to frustrating errors!

// Here is an example where we use a uniform variable in the fragment shader:

// #version 330 core
// out vec4 FragColor;GL_LINEAR_MIPMAP_LINEAR
//
// uniform vec4 ourColor;
//
// void main()
// {
//    FragColor = ourColor;
// }

   // Time in seconds since GLFW was initialized
// float timeValue  = glfwGetTime();

   // greenValue ranges from 0.0 to 1.0
// float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

   // Retrieve the location of the uniform ourColor from the given shader program
   // If the location of the uniform is not found, the function returns -1
// int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

   // To retrieve the location of a uniform variable from a shader program, we do not have to call glUseProgram before
   // But to update the value of a uniform variable, we do need to call glUseProgram first,
   // because the function used to we set the value of a uniform variable acts on the currently active shader program
// glUseProgram(shaderProgram);

   // Set the value of the uniform in the currently active shader program
   // The postfix of the glUniform call specifies the type of the uniform we want to set
   // A few of the possible postfixes are:
   //  f: float
   //  i: int
   // ui: unsigned int
   // 3f: 3 floats
   // fv: vector/array of floats -> Syntax: glUniform(location, count, array[count] or vec[count])
// glUniform4f(vertexColorLocation, // Location of uniform variable
//             0.0f,                // R
//             greenValue,          // G
//             0.0f,                // B
//             1.0f);               // A

// Up to this point we know how to:
// - Fill a VBO
// - Configure vertex attribute pointers
// - Store the configuration in a VAO
// Let's add color to the vertex data now

// When we supply 1 color for each vertex of a triangle, why do we end up with a gradient?
// The gradient you see is the result of something called fragment interpolation, which is done in the fragment shader
// The rasterization stage usually results in a number of fragment that is much larger than the number of vertices we provided
// The rasterizer determines the positions of the fragments based on where they reside on the triangle shape
// It then interpolates all the input variables of the fragment shader based on these positions

// Texturing:

// A texture is a 1D, 2D or 3D image used to add detail to an object.
// Aside from images, textures can also be used to store a large collection of data to send to shaders.
// In order to map a texture to a triangle, we need to tell each vertex of the triangle which part of the texture it corresponds to.
// Each vertex should thus have a texture coordinate associated with it that specifies what part of the texture image to sample from.
// Fragment interpolation then does the rest for all the fragments.

// Texture coordinates range from 0 to 1 in the x and y axes.
// Retrieving the color of a texture using coordinates is called sampling.
// Texture coordinates start at (0, 0) on the lower left corner of a texture image, and go up to (1, 1) at the upper right corner.

// For a triangle we only need to pass 3 texture coordinate points to the vertex shader, which passes them to the fragment shader.
// The fragment shader then interpolates all the texture coordinates for each fragment.

// Texture wrapping:

// Texture coordinates range from (0, 0) to (1, 1)
// What happens if we specify coordinates outside of this range?

// GL_REPEAT:          Repeat the texture image (default behaviour).
// GL_MIRRORED_REPEAT: Repeat the texture image but mirror it with each repeat.
// GL_CLAMP_TO_EDGE:   Clamp the coordinates between 0 and 1. This results in a streched edge pattern.
// GL_CLAMP_TO_BORDER: Coordinates outside the range are given a user-specified border color.

// Each mode can be set per coordinate axis (S, T and R = X, Y and Z):
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // Target, option and mode
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

// With the GL_CLAMP_TO_BORDER option we need to specify the border color:
// float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

// Texture filtering:

// Texture coordinates can be any floating point value, so OpenGL has to figure out what texture pixel (texel) corresponds to a given texture coordinate
// Texels are limited by the resolution, while texture coordinates are not.

// GL_NEAREST (Nearest Neighbor Filtering): Select the pixel whose center is closest to the texture coordinate (default behavior).
// GL_LINEAR ((Bi)linear filtering): Select an interpolated value, calculated using the texture coordinate's neighbording texels.

// Texture filtering can be set for magnifying and minifying operations (when scaling up or down)
// You could have GL_NEAREST when textures are scaled down, and GL_LINEAR when they are scaled up.

// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// Mipmaps:

// Say we have thousands of objects, each with an attached texture.
// The ones far away have the same high resolution texture as the ones close by.
// If a far away object only results in a few fragments,
// OpenGL has trouble retrieving the right color value for each fragment from the high resolution texture,
// since each fragment covers a large part of the texture image.
// This results in visual artifacts and wasted memory on high resolution textures.

// Solution: mipmaps.
// A mipmap is a collection of texture images, where each subsequent texture is half the size of the previous one.
// After a certain distance between the viewer and an object, OpenGL will use the mipmap texture that best suits the distance to the object.

// OpenGL might show visual artifacts like sharp edges between two mipmap levels.
// Just like normal texture filtering, it is also possible to filter between mipmap levels.
//    Texture        Mipmap
// GL_NEAREST_MIPMAP_NEAREST: Takes the nearest mipmap to match the pixel size and uses GL_NEAREST for texture sampling.
// GL_LINEAR_MIPMAP_NEAREST:  Takes the nearest mipmap level and samples using GL_LINEAR.
// GL_NEAREST_MIPMAP_LINEAR:  Linearly interpolates between the two mipmaps that most closely match the size of a pixel and samples via GL_NEAREST.
// GL_LINEAR_MIPMAP_LINEAR:   Linearly interpolates between the two closest mipmaps and samples the texture via GL_LINEAR.

// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// You cannot set one the mipmap filtering options as the magnification filter.
// Mipmaps are used for when textures get downscaled.
// Trying to the one of the mipmap filtering options as the magnification filter results in the GL_INVALID_ENUM error code.

// int width, height, nrChannels;
// unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

// unsigned int texture;
// glGenTextures(1, &texture);

// Bind so any subsequent texture commands will configure the currently bound texture
// glBindTexture(GL_TEXTURE_2D, texture);

// Now that the texture is bound, we can generate a texture using the previously loaded image data:

// glTexImage2D generates a texture image on the currently bound texture object at the active texture unit
// glTexImage2D(GL_TEXTURE_2D,    // target:         Common ones are GL_TEXTURE_1D, 2D or 3D.
//              0,                // level:          Specifies the level-of-detail number.
                                  //                 Level 0 is the base image level.
                                  //                 Level n is the nth mipmap reduction image.
//              GL_RGB,           // internalFormat: Specifies the number of color components in the texture.
                                  //                 So it tells OpenGL the format in which we want to store the texture.
//              width,            // Width
//              height,           // Height
//              0,                // Border:         Legacy value, must be equal to 0.
//              GL_RGB,           // format:         Specifies the format of the pixel data.
//              GL_UNSIGNED_BYTE, // type:           Specifies the type of the pixel data.
//              data);            // data:           Array of bytes.

// Now the currently bound texture object has the texture image attached to it.
// But it only has the base-level of the texture image loaded.
// If we want to use mipmaps, we have to specify all the different images manually by continually increasing the 2nd argument.
// Or we can call glGenerateMipMap after generating the texture:

// Automatically generates all the mipmap levels for the currently bound texture image of the active texture unit.
// Each subsequent mipmap image is halved until the mipmap image has a width or height of 1 pixel.
// glGenerateMipmap(GL_TEXTURE_2D);

// After generating the texture and its mipmaps, we can free the image memory
// stbi_image_free(data);

// So generating a texture looks like this:

// unsigned int texture;
// glGenTextures(1, &texture);
// glBindTexture(GL_TEXTURE_2D, texture);
// // set the texture wrapping/filtering options (on the currently bound texture object)
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// // load and generate the texture
// int width, height, nrChannels;
// unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
// if (data)
// {
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//    glGenerateMipmap(GL_TEXTURE_2D);
// }
// else
// {
//    std::cout << "Failed to load texture" << std::endl;
// }
// stbi_image_free(data);

// Now to apply a texture to a rectangle:

//                     Position             Color               TexCoords
//                    <--------------->    <-------------->    <-------->
// GLfloat data[] = { 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top right
//                    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom right
//                   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom left
//                   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f }; // Top left

// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
// glEnableVertexAttribArray(0);
// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
// glEnableVertexAttribArray(1);
// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
// glEnableVertexAttribArray(2);

// The vertex shader must forward the texture coordinates to the fragment shader

// #version 330 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
// layout (location = 2) in vec2 aTexCoord;
// 
// out vec3 ourColor;
// out vec2 TexCoord;
// 
// void main()
// {
//    gl_Position = vec4(aPos, 1.0);
//    ourColor = aColor;
//    TexCoord = aTexCoord;
// }

// How do we pass the texture object to the fragment shader? Using the GLSL built-in data-type sampler2D.
// To sample the color of a texture we use GLSL's built in texture function. The texture function samples the color value from the texture using the nearest/linear options we set earlier.

// #version 330 core
// out vec4 FragColor;
// 
// in vec3 ourColor;
// in vec2 TexCoord;
// 
// uniform sampler2D ourTexture;
// 
// void main()
// {
//    FragColor = texture(ourTexture, TexCoord);
// }

// All that is left is to bind the texture before we draw:

// glBindTexture(GL_TEXTURE_2D, texture);
// glBindVertexArray(VAO);
// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

// Texture units:

// Why is the sampler2D variable a uniform if we didn't assign it some value with glUniform?
// Using glUniform1i we can assign a location value to the texture sampler, so that we can have multiple textures at once in a fragment shader.
// The location of a shader is known as its texture unit.
// The default texture unit for a texture is 0, which is also the default active texture unit.
// That's why we did not have to assign a location to our texture.

// Texture units allow us to have more than 1 texture in our shaders.
// By assigning texture units to the samplers, we can bind to multiple textures at once as long as we activate the corresponding texture unit first.
// Just like glBindTexture we can activate texture units using glActiveTexture:

// glActiveTexture(GL_TEXTURE0); // Activate the texture unit
// glBindTexture(GL_TEXTURE_2D, texture);

// After activating a texture unit, a subsequent glBindTexture call will bind that texture to the currently active texture unit.
// GL_TEXTURE0 is activated by default.
// OpenGL should have a minimum of 16 texture units.

// To accept another sampler in the fragment shader:

// #version 330 core
// ...
// 
// uniform sampler2D texture1;
// uniform sampler2D texture2;
// 
// void main()
// {
//    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
// }

// The mix function takes two values as input and linearly interpolates between them based on its third argument.
// If the 3rd argument is 0.0, the function returns the first input.
// If it is 1.0, it return the second input.
// If it is 0.2, it return 80% of the first input and 20% of the second input.

// This is how we give the two textures to the shader:

// ... Generate the two textures ...
// Tell OpenGL how to connect the texture samplers with the texture units using glUniform1i:

// ourShader.use()                                                 // Using our Shader class
// glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // Texture unit 0
// ourShader.setInt("texture2", 1);                                // Texture unit 1

// Inside the render loop:
// glActiveTexture(GL_TEXTURE0);
// glBindTexture(GL_TEXTURE_2D, texture1);
// glActiveTexture(GL_TEXTURE1);
// glBindTexture(GL_TEXTURE_2D, texture2);
// 
// glBindVertexArray(VAO);
// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

// Texture is flipped upside down?
// OpenGL expects the 0.0 coordinate of the Y-axis to be on the bottom side of the image,
// but images usually have 0.0 at the top of the Y-axis?
// The stb_image library has a setting for flipping images while loading them:

// stbi_set_flip_vertically_on_load(true);

// Coordinate systems

// Vertices must be in NDC after the vertex shader is executed
// We usually specify the vertex coordinates in a range we configure ourselves,
// and then we transform these coordinates to NDC in the vertex shader
// The NDC are then given to the rasterizer, which transforms them to 2D coordinates/pixels on the screen

// Vertex coordinates -> NDC -> Screen coordinates
// Transformations done in a step-by-step fashion
// The advantage of transforming an object vertices to several intermediate coordinate systems is that
// some of the calculations are easier to perform in certain coordinate systems

// Five coordinate systems:
// 1) Local space or object space
// 2) World space
// 3) View space or eye space
// 4) Clip space
// 5) Screen space

// Let's examine each one in detail:

// 1) Local space:
// Local coordinates are the coordinates of an object relative to its local origin
// All the vertices of an object are in local space

// 2) World space:
// World coordinates are coordinates that are specified relative to the global origin of the world
// The model matrix transforms local coordinates into world coordinates
// The model matrix is a transformation matrix that translates, rotates and scales objects to give them
// a specific location/orientation in the world

// 3) View space:
// Also known as the camera space of the eye space
// The view space is the result of transforming world-space coordinates to coordinates that are in front
// of the user's view
// The view matrix transforms world coordinates to camera coordinates

// 4) Clip space:
// At the end of each vertex shader run, OpenGL expects the coordinates to be within a specific range.
// Any coordinate that falls outside of this range is clipped.
// Coordinates that are clipped are discarded.

// Because specifying the visible coordinates to be within the range -1.0 and 1.0 isn't intuitive,
// we specify our own coordinate range to work with and then convert the coordinates in that range to NDC

// To transform vertex coordinates from view space to clip space, we define a projection matrix that
// specifies a range of coordinates in each dimension. The projection matrix then transforms the coordinates
// within this specified range to NDC (-1.0, 1.0).
// All coordinates outside the specified range will not be mapped between -1.0 and 1.0, so they will be clipped.

// Example: If we specify the range [-1000, 1000] in the X, Y and Z axes, a coordinate of (1250, 500, 750)
// would not be visible, since the X coordinate is outside of the range and thus gets converted to a
// coordinate higher than 1.0 in NDC.

// Note that if only a part of a primitive e.g. a triangle is outside of the clipping volume,
// OpenGL will reconstruct the triangle as one or more triangles to fit inside the clipping range.

// The viewing box that a projection matrix creates is called a frustum
// Each coordinate inside this frustum will end up on the user's screen
// The total process of converting coordinates within a specified range to NDC that can be easily mapped
// to 2D view-space coordinates is called projection, since the projection matrix projects 3D coordinates
// to easy-to-map-to-2D normalized device coordinates

// Once all the vertices are transformed to the clip space, a final operation called perspective division
// is performed, where we divide the X, Y and Z components of the position vectors by the vector's
// homogenous W component.
// Perspective division is what transforms the 4D clip space coordinates to 3D normalized device coordinates
// This step is performed automatically at the end of each vertex shader run
// It is after this stage where the resulting coordinates are mapped to screen coordinates
// using the settings of glViewport, and turned into fragments

// The projection matrix that transforms view coordinates to clip coordinates can take two different forms,
// where each form defines its own unique frustum:

// 1) Orthographic projection:
// An ortographic projection matrix defines a cube-like frustum
// Because of this, when we create an ortographic projection matrix we need to specify the width,
// height and length of the visible frustum
// All the coordinates that end up inside this frustum after being transformed from the view space to the
// clip space will not be clipped

// The dimensions of the frustum are specified with a width, a height, a near plane and a far plane.
// The ortographic frustum directly maps all coordinates inside the frustum to NDC since the W
// component of each vector is untouched
// If the W component is equal to 1.0, perspective division does not change the coordinates

// glm::ortho(GLint left, GLint right, GLint bottom, GLint top, GLfloat near, GLfloat far)
// glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

// An ortographic projection matrix directly maps coordinates to the 2D plane that is the screen
// This produces unrealistic results since the projection doesn't take perspective into account

// 2) Perspective projection:
// Objects that are far away are smaller than objects that are nearby
// Parallel lines seem to converge at infinity
// A projection matrix maps a given frustum range to clip space, but it also manipulates the W component
// of each vertex coordinate in such a way that the further away a vertex coordinate is from the viewer,
// the higher this W component becomes.
// Once the coordinates are transformed to the clip space, they are in the range -W to W
// Anything outside this range is clipped.
// OpenGL requires that the visible coordinates fall between -1.0 and 1.0, thus once the coordinates are
// in the clip space, perspective division is applied
// Each component of the each vertex coordinate is divided by its W component, resulting in smaller vertex
// coordinates the further away a vertex is from the viewer
// This is another reason why the W component is important, aside from the fact that it enables translations
// The resulting coordinates are then in normalized device space

// glm::mat4 proj = glm::perspective(glm::radians(45.0f),          // FoV
//                                   (float)width / (float)height, // Aspect ratio
//                                   0.1f,                         // Near
//                                   100.0f);                      // Far

// Whenever the near value of your perspective matrix is set a bit too high (like 10.0f),
// OpenGL will clip all coordinates close to the camera (between 0.0f and 10.0f),
// which gives a familiar visual result in videogames where you can see through certain objects
// if you move too close to them.

// Ortographic projection -> Each of the vertex coordinates is directly mapped to clip space
//                           The W component is not manipulated, so perspective division has no effect
//                           So objects far away do not seem smaller
//                           Good for 2D renderings and architectural and engineering applications

// V_clip = M_proj * M_view * M_model * V_local
// Result should be assigned to gl_Position in the vertex shader
// OpenGL handles the perspective division and clipping

// So vertex shader outputs clip space coordinates
// OpenGL then transforms those coordinates to NDC by doing the perspective division
// Finally, OpenGL uses the parameters from glViewport to map the NDC to screen coordinates, where each
// coordinate corresponds to a point on the screen. This process is called the viewport transform.

// Going 3D

// Say we want to move the camera backwards
// This the same as moving the scene forward
// Which is exactly what the view matrix does
// Because we want to move backwards and because OpenGL uses a right-handed coordinate system,
// we need to move the camera in the positive Z-axis direction.
// So the scene must move in the negative Z-axis direction.
// Note that in NDC OpenGL actually uses a left-handed coordinate system (the projection matrix switches the handedness)

// Z-buffer

// OpenGL stores all its depth information in what is called the z-buffer.
// GLFW automatically creates a z-buffer, just like it creates a color-buffer to store the colors of the output image
// The depth is stored within each fragment
// When a fragment is going to be displayed, OpenGL compares the depth of the fragment with the depth stored in the z-buffer
// If the depth of the current fragment is greater than the depth stored in the z-buffer, the fragment is discarded
// If it is smaller, the fragment is displayed
// This process is called depth testing

// If we want OpenGL to perform depth testing, we need to enable it (it is disabled by default)
// We can enable depth testing using glEnable:

// glEnable(GL_DEPTH_TEST);

// We must also clear the z-buffer for each iteration:

// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Camera

// The view matrix transforms all the world coordinates into view coordinates,
// which are specified relative to the camera's position and orientation

// A) From point:
// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); 

// B) At point:
// glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

// C) Direction:
// glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

// Why is the direction pointing towards the camera?

// D) Right axis:

// glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
// glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

// E) Up axis:

// glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

// We just built a coordinate frame! This process is known as the Gram-Schmidt process.

// The function glm::lookAt does all this for us:

// glm::mat4 view = glm::lookAt(from,
//                              at,
//                              up);

// Movement speed

// Using a constant value for movement speed results in different speeds on different systems
// A fast system draws more frames than a slow one
// Each time a frame is drawn, the position of the camera is updated
// So a system that draws more frames also updates the position of the camera more often,
// resulting in faster movement

// We want the speed to be the same on all systems
// To do this, we can keep track of a deltatime variable that stores the time it takes to render the
// last frame, and we can multiply all the velocities with this deltatime value
// The result is that when we have a large deltaTime in a frame, meaning that the last frame took longer
// than average to render, the velocity for the current frame will be faster to balance things out

// To do this, we use two global variables:

// float deltaTime = 0.0f; // Time between current frame and last frame
// float lastFrame = 0.0f; // Time of last frame

// We calculate the new deltaTime in each frame:

// float currentFrame = glfwGetTime();
// deltaTime          = currentFrame - lastFrame;
// lastFrame          = currentFrame;

// Lighting

// What variables do we need to calculate the diffuse and specular lighting?
// 1) The position of the light source
// 2) The position of the fragment being considered
// 3) The normal of the fragment being considered
// 4) The position of the camera (for specular lighting only)


// When doing the lighting calculations in world space, we need item 2 to be in world space
// We can accomplish this by multiplying the vertex position attribute with the model matrix only,
// and passing it to the fragment shader

// So our vertex shader would look like this:

// out vec3 FragPos;
// out vec3 Normal;
//
// void main()
// {
//    gl_Position = projection * view * model * vec4(aPos, 1.0);
//    FragPos = vec3(model * vec4(aPos, 1.0));
//    Normal = aNormal;
// }

// And our fragment shader would look like this:

// in vec3 Normal;
// in vec3 FragPos;
// uniform vec3 lightPos;

// vec3 norm     = normalize(Normal);
// vec3 lightDir = normalize(lightPos - FragPos);
// float diff    = max(dot(norm, lightDir), 0.0);
// vec3 diffuse  = diff * lightColor;
// vec3 result   = (ambient + diffuse) * objectColor;
// FragColor     = vec4(result, 1.0);

// When doing lighting calculations, make sure you normalize the relevant vectors

// Note:
// 1) Translations should not affect normal vectors.
// 2) If the model matrix contains a non-uniform scale,
//    the normal vectors would be transformed incorrectly by it.

// So we can't apply the model matrix to normal vectors directly
// We must first remove the translation part from the model matrix by taking the upper-left 3x3 part,
// and we must also take care of the non-uniform scale
// The resulting matrix is called the normal matrix
// It is defined as: "the transpose of the inverse of the upper-left 3x3 part of the model matrix"

// Normal = mat3(transpose(inverse(model))) * aNormal;

// Inverting matrices is very expensive
// Always try to avoid doing inversions in shaders
// Do the inverse on the GPU, and pass it to the shaders via a uniform

// Most people do lighting calculations in the view space instead of the world space
// The advantage of doing calculations in the view space is that the camera's position is always at
// (0, 0, 0), which means that we get this variable for free

// For specular lighting:

// uniform vec3 viewPos;
// float specularStrength = 0.5;
// vec3 viewDir    = normalize(viewPos - FragPos);
// vec3 reflectDir = reflect(-lightDir, norm); // The reflect function expects the vector to point from
                                               // the light source to the fragment, which is the opposite of what we have
// float spec    = pow(max(dot(viewDir, reflectDir), 0.0), 32);
// vec3 specular = specularStrength * spec * lightColor;
// vec3 result   = (ambient + diffuse + specular) * objectColor;
// FragColor     = vec4(result, 1.0);

// Developers used to implement the Phong lighting model in the vertex shader
// This was advantageous because there are normally less vertices that fragments,
// so the lighting calculations would be performed less times
// But the colors of the fragments would then be interpolated from the colors of the vertices,
// resulting in an unrealistic look unless many vertices were used
// When the Phong lighting model is implemented in the vertex shader it is called Gourad Shading

// Lighting maps

// We have been defining materials for entire objects
// But objects are composed of many different materials
// The diffuse and specular components vary over the surface of an object
// This is what we want to simulate using diffuse and specular maps

// 1) Diffuse map:
// We want to set the diffuse color of an object for each individual fragment
// So we want to retrieve the diffuse color based on a fragment's position on the object -> Use a texture!
// Using a diffuse map is just like using a texture

// We will replace the diffuse vec3 with a sampler2D
// Keep in mind that sampler2D is an opaque type, which means it can't be instantiated
// It can only be defined as a uniform
// Since we will put the sampler2D inside a struct, the struct must be a uniform

// 2) Specular map:
// We want to set the specular color of an object for each individual fragment
// So we want to retrieve the specular color based on a fragment's position on the object -> Use a texture!
// Using a specular map is just like using a texture

// We will replace the specular vec3 with a sampler2D
// No highlights can appear in the black parts of a specular map
// Use Photoshop or Gimp to modify a diffuse texture and turn it into a specular map

// Using a specular map we can specify what parts of an object are shiny
// You can use colors in the specular map to also determine the color of the specular highlights

// 3) Emission map:
// An emission map is a texture that stores emission values per fragment
// Emission values are colors an object emits as if it contained a light source

// Light sources

// 1) Directional light
// In lighting calculations the light direction points from the fragment to the light source
// The direction of a directional light is usually specified in the opposite way:
// The light direction points from the light source to the fragment
// So we need to invert the direction of a directional light when doing lighting calculations

// Note: We've been passing the light's position and direction vectors as vec3s, but some people
//       prefer to keep all vectors defined as vec4s.
//       If you decide to do this, remember that position vectors must have a w component equal to 1.0 so
//       that translations and projections apply to them, and that direction vectors must have a w
//       component equal to 0.0 so that translations don't affect them
//       A cool use case of this is:
//       if (lightVector.w == 0.0)      { // Do directional light calculations }
//       else if (lightVector.w == 1.0) { // Do point light calculations }

// 2) Point light

// The light emitted by a point light can be attenuated

// Att = 1.0 / (Kc + (Kl * d) + (Kq * d^2))

// Stencil Testing

// ...

// Blending

// Alpha = Opaqueness
// Alpha = 1.0 = Fully opaque
// Alpha = 0.0 = Fully transparent

// OpenGL doesn't know what to do with alpha values, nor when to discard a fragment based on its alpha value.
// This is something we need to do manually using the discard command.

// #version 330 core
// out vec4 FragColor;
//
// in vec2 TexCoords;
// 
// uniform sampler2D texture1;
// 
// void main()
// {
//     vec4 texColor = texture(texture1, TexCoords);
//     if(texColor.a < 0.1)
//         discard;
//     FragColor = texColor;
// }

// Note that when sampling textures at their borders, OpenGL interpolates the border values with the
// next repeated value of the texture if we set the wrapping parameters of the texture to GL_REPEAT).
// This is usually okay, but if the texture has a fully transparent part at one border, and a fully opaque
// part at the opposite border, then the transparent part will be interpolated with the opaque one, losing its
// transparency. To prevent this, set the texture wrapping method to GL_CLAMP_TO_EDGE whenever you
// use alpha textures.

// glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

// To enable blending:

// glEnable(GL_BLEND);

// Blending is done with the following equation:

// Color_result = Color_src * Factor_src + Color_dest * Factor_dest

// Csrc:  the source color vector. This is the color vector that originates from the texture.
// Cdest: the destination color vector. This is the color vector that is currently stored in the color buffer.
// Fsrc:  the source factor value. Sets the impact of the alpha value on the source color.
// Fdest: the destination factor value. Sets the impact of the alpha value on the destination color.

// After the fragment shader has run and all the tests have passed, the result of this blend equation is
// calculated using the fragment shader's color output and whatever is currently in the color buffer
// (the previous fragment color). The source and destination colors will automatically be set by OpenGL,
// but the source and destination factors can be set by us.

// Set the factors with:

// glBlendFunc(GLenum sfactor, GLenum dfactor)

// GL_ZERO                      Factor is equal to 0.
// GL_ONE                       Factor is equal to 1.
// GL_SRC_COLOR                 Factor is equal to the source color vector Csource.
// GL_ONE_MINUS_SRC_COLOR       Factor is equal to 1 minus the source color vector: 1-Csource.
// GL_DST_COLOR                 Factor is equal to the destination color vector Cdestination
// GL_ONE_MINUS_DST_COLOR       Factor is equal to 1 minus the destination color vector: 1-Cdestination.
// GL_SRC_ALPHA                 Factor is equal to the alpha component of the source color vector Csource.
// GL_ONE_MINUS_SRC_ALPHA       Factor is equal to 1-alpha of the source color vector Csource.
// GL_DST_ALPHA                 Factor is equal to the alpha component of the destination color vector Cdestination.
// GL_ONE_MINUS_DST_ALPHA       Factor is equal to 1-alpha of the destination color vector Cdestination.
// GL_CONSTANT_COLOR            Factor is equal to the constant color vector Cconstant.
// GL_ONE_MINUS_CONSTANT_COLOR  Factor is equal to 1 - the constant color vector Cconstant.
// GL_CONSTANT_ALPHA            Factor is equal to the alpha component of the constant color vector Cconstant.
// GL_ONE_MINUS_CONSTANT_ALPHA  Factor is equal to 1-alpha of the constant color vector Cconstant.

// Example:
// Fully opaque red (destination) is in the buffer (alpha = 1.0)
// Transparent green (source) needs to be drawn in the same place (alpha = 0.6)
// Fsrc  = 0.6     = 60% green
// Fdest = 1 - 0.6 = 40% red
// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// It is also possible to set different options for the RGB and alpha channels individually
// using glBlendFuncSeparate:
// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

// OpenGL gives us even more flexibility by allowing us to change the operator between the source
// and destination part of the equation. Right now, the source and destination components are added together,
// but we could also subtract them if we want. glBlendEquation(GLenum mode) allows us to set this operation
// and has 3 possible options:

// GL_FUNC_ADD:              the default, adds both components to each other: Cresult=Src+Dst.
// GL_FUNC_SUBTRACT:         subtracts both components from each other: Cresult=Src-Dst.
// GL_FUNC_REVERSE_SUBTRACT: subtracts both components, but reverses order: Cresult=Dst-Src.

// When drawing a scene with opaque and transparent objects, the general outline is usually as follows:

// 1) Draw all opaque objects first.
// 2) Sort all the transparent objects.
// 3) Draw all the transparent objects in sorted order.

// The transparent objects must be sorted based on their distance from the camera:

// Sort in ascending order
// std::map<float, glm::vec3> sorted;
// for (unsigned int i = 0; i < windows.size(); i++)
// {
//     float distance = glm::length(camera.Position - windows[i]);
//     sorted[distance] = windows[i];
// }

// Retrieve from farthest to nearest
// for(std::map<float,glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) 
// {
//     model = glm::mat4(1.0f);
//     model = glm::translate(model, it->second);
//     shader.setMat4("model", model);
//     glDrawArrays(GL_TRIANGLES, 0, 6);
// } 

// Note that using glm::length2 is more efficient (no square root)

// Note that with fully transparent objects like grass leaves, we have the option to simply discard
// the transparent fragments instead of blending them, which means we don't have to order them (no depth issues).

// Face Culling

// If we imagine any closed shape, each of its faces has two sides. Each side would either face the user or
// show its back to the user. What if we could only render the faces that are facing the viewer?
// This is exactly what face culling does. OpenGL renders all the faces that are front-facing
// and discards all the faces that are back-facing, saving us a lot of fragment shader calls.

// We need to tell OpenGL which of the faces we use are front-facing and which faces are the back-facing.
// We do this through the winding order of the vertex data.

// By default, triangles defined with counter-clockwise vertices are processed as front-facing triangles.

// When defining the vertex order of a triangle, you visualize it as if it was facing you,
// so the vertices of each triangle you specify should be counter-clockwise.
// The cool thing about specifying all your vertices like this is that the actual winding order is calculated
// at the rasterization stage (when the vertex shader has already run). The vertices are then seen as from
// the viewer's point of view.

// All the triangle vertices that the viewer is facing are indeed in the correct winding order, as we specified them,
// but the vertices of the triangles at the other side of the cube are now rendered in such a way
// that their winding order becomes reversed. The result is that the triangles we're facing are seen as
// front-facing triangles, and the triangles at the back are seen as back-facing.

// glEnable(GL_CULL_FACE); // From this point on, all the faces that are not front-faces are discarded

// Note that we would have to disable face culling when we draw the grass leaves from the previous tutorial,
// for example, since their front and back faces should be visible.

// glCullFace(GL_FRONT); // Select which face you want to cull: GL_FRONT, GL_BACK, GL_FRONT_AND_BACK

// glFrontFace(GL_CCW); // Define which vertex ordering corresponds to a front-facing triangle


// Framebuffers

// So far we have used:
// Color buffer for writing color values
// Depth buffer for writing depth information
// Stencil buffer for discarding certain fragments based on some condition

// The combination of these buffers is called a framebuffer and is stored somewhere in memory.
// OpenGL gives us the flexibility to define our own framebuffers 

// The rendering operations we've done so far were all done on top of the render buffers
// that are attached to the default framebuffer.
// The default framebuffer is created and configured when you create your window (GLFW does this for us).

// To create a framebuffer object (FBO):

// unsigned int fbo;
// glGenFramebuffers(1, &fbo);

// To bind a framebuffer object:

   // There are 3 framebuffer targets:
   // GL_FRAMEBUFFER:      By binding to this target, all read and write framebuffer operations will affect
   //                      the currently bound framebuffer.
   // GL_READ_FRAMEBUFFER: Only the read operations affect the framebuffer bound to this target.
   //                      Example: glReadPixels()
   // GL_DRAW_FRAMEBUFFER: Only the write operations affect the framebuffer bound to this target.
   //                      Example: Destination for rendering, clearning and other write operations.
// glBindFramebuffer(GL_FRAMEBUFFER, fbo);

// Unfortunately, we can't use our framebuffer yet because it is not complete.
// For a framebuffer to be complete the following requirements have to be satisfied:

// 1) We have to attach at least one buffer (color, depth or stencil buffer).
// 2) There should be at least one color attachment.
// 3)All attachments should be complete as well (reserved memory).
// 4) Each buffer should have the same number of samples.

// For a definition of samples, take a look at the Anti Aliasing section.

// After we have met all the requirements, we can check if we successfully completed the framebuffer
// by calling glCheckFramebufferStatus with GL_FRAMEBUFFER, which checks the framebuffer that is currently
// bound to GL_FRAMEBUFFER and returns GL_FRAMEBUFFER_COMPLETE if everything is good.

// if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)

// All subsequent rendering operations will now render to the attachments of the currently bound framebuffer.
// Since our framebuffer is not the default framebuffer, the rendering commands will have no impact
// on the visual output of the window. For this reason it is called off-screen rendering when we render
// to a framebuffer that is not the default one.

// To make sure all rendering operations will have a visual impact on the main window,
// we need to make the default framebuffer active again by binding to 0:

// glBindFramebuffer(GL_FRAMEBUFFER, 0);

// When we're done with all framebuffer operations, do not forget to delete the framebuffer object:

// glDeleteFramebuffers(1, &fbo);

// Now, before the completeness check is executed we need to attach one or more attachments to the framebuffer.
// An attachment is a memory location that can act as a buffer for the framebuffer. Think of it as an image.
// When creating an attachment we have two options to take: textures or renderbuffer objects.

// Texture attachments:
// -------------------

// When attaching a texture to a framebuffer, all rendering commands will write to the texture as if it was
// a normal color/depth/stencil buffer.
// The advantage of using textures is that the result of all rendering operations will be stored as a texture
// image that we can then easily use in our shaders.

// Creating a texture for a framebuffer is roughly the same as creating a normal texture:

// unsigned int texture;
// glGenTextures(1, &texture);
// glBindTexture(GL_TEXTURE_2D, texture);

// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// The main differences here are that we set the dimensions equal to the screen size (although this is not required),
// and we pass NULL as the texture's data parameter. For this texture, we're only allocating memory and not
// actually filling it. The texture will be filled as soon as we render to the framebuffer.
// Also note that we do not care about any of the wrapping methods or mipmapping since we won't be needing
// those in most cases.

// If you want to render your whole screen to a texture of a smaller or larger size, you need to call
// glViewport again (before rendering to your framebuffer) with the new dimensions of your texture.
// Otherwise only a small part of the texture or screen will be drawn onto the texture.

// Now that we've created a texture the last thing we need to do is actually attach it to the framebuffer:

// glFramebufferTexture2D(GL_FRAMEBUFFER,       // target:     framebuffer type we are targeting (R, W, RW)
//                        GL_COLOR_ATTACHMENT0, // attachment: type of attachment we are attaching. Options:
                                                //             GL_COLOR_ATTACHMENTi
                                                //             GL_DEPTH_ATTACHMENT
                                                //             GL_STENCIL_ATTACHMENT
                                                //             GL_DEPTH_STENCIL_ATTACHMENT
//                        GL_TEXTURE_2D,        // textarget:  type of texture you want to attach.
//                        texture,              // texture     texture to attach.
//                        0);                   // level:      mipmap level.

// When creating a depth texture, the texture's format and internalformat should be GL_DEPTH_COMPONENT.
// In the case of a stencil texture, they should be GL_STENCIL_INDEX.

// It is also possible to attach both a depth buffer and a stencil buffer as a single texture.
// Each 32 bit value of the texture then consists of 24 bits of depth information and 8 bits of
// stencil information. To attach a depth and stencil buffer as one texture we use the
// GL_DEPTH_STENCIL_ATTACHMENT type and configure the texture's formats to contain combined depth and stencil
// values. Example:

// glTexImage2D(GL_TEXTURE_2D,
//              0,
//              GL_DEPTH24_STENCIL8,
//              800,
//              600,
//              0,
//              GL_DEPTH_STENCIL,
//              GL_UNSIGNED_INT_24_8,
//              NULL);
// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

// Renderbuffer object attachments:
// -------------------------------

// Just like a texture image, a renderbuffer object is an actual buffer e.g. an array of bytes, integers,
// pixels or whatever. A renderbuffer object has the added advantage though that it stores its data in OpenGL's
// native rendering format making it optimized for off-screen rendering to a framebuffer.

// Renderbuffer objects store all the render data directly into their buffer without any conversions
// to texture-specific formats, thus making them faster as a writeable storage medium.
// However, renderbuffer objects are generally write-only, thus you cannot read from them
// (like with texture-access). It is possible to read from them via glReadPixels, though that returns
// a specified area of pixels from the currently bound framebuffer, but not directly from the attachment itself.

// Because their data is already in its native format, they are quite fast when writing data or simply
// copying their data to other buffers. Operations like switching buffers are thus quite fast when using
// renderbuffer objects. The glfwSwapBuffers function we've been using at the end of each render iteration
// might as well be implemented with renderbuffer objects: we simply write to a renderbuffer image,
// and swap to the other one at the end. Renderbuffer objects are perfect for these kind of operations.

// To create a renderbuffer object (RBO) and bind it so that all subsequent renderbuffer operations affect it:

// unsigned int rbo;
// glGenRenderbuffers(1, &rbo);
// glBindRenderbuffer(GL_RENDERBUFFER, rbo);

// Since renderbuffer objects are generally write-only they are often used as depth and stencil attachments,
// since most of the time we don't really need to read values from the depth and stencil buffers but still care
// about depth and stencil testing. We need the depth and stencil values for testing, but we don't need to
// sample these values so a renderbuffer object suits this perfectly. When we're not sampling from these
// buffers, a renderbuffer object is generally preferred since it's more optimized.

// Creating a depth and stencil RBO is done by calling the glRenderbufferStorage function.
// This function establishes the data storage, format and dimensions of an RBO's image.
// It only allocates memory, it doesn't fill the buffer.

// glRenderbufferStorage(GL_RENDERBUFFER,     // target: the RBO that is currently bound to this target
                                              //         is the one that is configured.
//                       GL_DEPTH24_STENCIL8, // internalformat: specifies the internal format to be used for
                                              //                 the RBO's image.
//                       800,                 // width: width of RBO's image in pixels.
//                       600);                // height: height of RBO's image in pixels.

// Finally, to attach an RBO to the currently bound framebuffer object:

// glFramebufferRenderbuffer(GL_FRAMEBUFFER,              // target: framebuffer target.
//                           GL_DEPTH_STENCIL_ATTACHMENT, // attachment: attachment point of FB.
//                           GL_RENDERBUFFER,             // renderbuffertarget: RB target.
//                           rbo);                        // renderbuffer: actual RB to attach.

// Renderbuffer objects could provide some optimizations in your framebuffer projects, but it is important
// to realize when to use renderbuffer objects and when to use textures. The general rule is that if you
// never need to sample data from a specific buffer, it is wise to use a renderbuffer object for that
// specific buffer. If you need to someday sample data from a specific buffer like colors or depth values,
// you should use a texture attachment instead.

// Rendering to a texture:
// ----------------------

// To render the scene into a color texture attached to a FBO we created:

// unsigned int framebuffer;
// glGenFramebuffers(1, &framebuffer);
// glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

   // Create a texture image to be used as a color attachment
// unsigned int texColorBuffer;
// glGenTextures(1, &texColorBuffer);
// glBindTexture(GL_TEXTURE_2D, texColorBuffer);
// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// glBindTexture(GL_TEXTURE_2D, 0);

   // Attach the texture image to the currently bound FBO as a color attachment
// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0); 

   // Create a RBO to be used as a depth/stencil attachment
   // We use a RBO instead of a texture image because we won't be sampling the depth/stencil buffers
// unsigned int rbo;
// glGenRenderbuffers(1, &rbo);
// glBindRenderbuffer(GL_RENDERBUFFER, rbo);
// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
// glBindRenderbuffer(GL_RENDERBUFFER, 0);

   // Attach the RBO to the currently bound FBO as a depth/stencil attachment
// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

   // Finally, check that the FBO is complete and we unbind it
// if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
// glBindFramebuffer(GL_FRAMEBUFFER, 0);

// Now that the framebuffer is complete all we need to do to render to the framebuffer's buffers instead
// of the default framebuffer's buffers is to simply bind the framebuffer object. All subsequent rendering
// commands will then influence the currently bound framebuffer. All the depth and stencil operations will
// also read from the currently bound framebuffer's depth and stencil attachments if they're available.
// If you were to omit a depth buffer for example, all depth testing operations will no longer work,
// because there isn't a depth buffer present in the currently bound framebuffer.

// So, to draw the scene to a single texture we'll have to complete the following steps:

// 1) Render the scene as usual with the new framebuffer bound as the active framebuffer.
// 2) Bind the default framebuffer.
// 3) Draw a quad that spans the entire screen with the new framebuffer's color buffer as its texture.

// When drawing to the quad the spans the entire screen, there is no need for a projection matrix if we
// specify the vertex coordinates of the quad as Normalized Device Coordinates, which allows us to output
// them from the vertex shader directly.

// Our rendering loop looks like this:

   // first pass
// glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
// glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
// glEnable(GL_DEPTH_TEST);
// DrawScene();

   // second pass
// glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default framebuffer
// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
// glClear(GL_COLOR_BUFFER_BIT); // we'are not using the depth buffer since we are working with a single quad

// screenShader.use();
// glBindVertexArray(quadVAO);
// glDisable(GL_DEPTH_TEST);
// glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
// glDrawArrays(GL_TRIANGLES, 0, 6);

// Post-processing:
// ---------------

// Now that the entire scene is rendered to a single texture we can create some interesting effects simply by manipulating
// the texture data.

// Inversion:
// +++++++++

// void main()
// {
//     FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
// }

// Grayscale:
// +++++++++

// void main()
// {
//     FragColor = texture(screenTexture, TexCoords);
//     float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//     FragColor = vec4(average, average, average, 1.0);
// }

// The human eye tends to be more sensitive to green colors and less sensitive to blue colors, so to get the most physically
// accurate results we must use weighted channels:

// void main()
// {
//     FragColor = texture(screenTexture, TexCoords);
//     float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//     FragColor = vec4(average, average, average, 1.0);
// }

// Kernel effects:
// ++++++++++++++

// We could for example take a small area around the current texture coordinate and sample multiple texture values around it.

// A kernel (or convolution matrix) is a small matrix-like array of values centered on the current pixel that multiplies
// surrounding pixel values by its kernel values and adds them all together to form a single value.

// Most kernels add up to 1 if you add all the weights together. If they don't add up to 1 it means that the resulting texture color
// is brighter or darker than the original texture value.

// Sharpen kernel:

// const float offset = 1.0 / 300.0;
// 
// void main()
// {
//     vec2 offsets[9] = vec2[](
//         vec2(-offset,  offset), // top-left
//         vec2( 0.0f,    offset), // top-center
//         vec2( offset,  offset), // top-right
//         vec2(-offset,  0.0f),   // center-left
//         vec2( 0.0f,    0.0f),   // center-center
//         vec2( offset,  0.0f),   // center-right
//         vec2(-offset, -offset), // bottom-left
//         vec2( 0.0f,   -offset), // bottom-center
//         vec2( offset, -offset)  // bottom-right
//         );
// 
//     float kernel[9] = float[](
//         -1, -1, -1,
//         -1,  9, -1,
//         -1, -1, -1
//         );
// 
//     vec3 sampleTex[9];
//     for(int i = 0; i < 9; i++)
//     {
//         sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
//     }
//     vec3 col = vec3(0.0);
//     for(int i = 0; i < 9; i++)
//         col += sampleTex[i] * kernel[i];
// 
//     FragColor = vec4(col, 1.0);
// }

// Blur kernel:

// float kernel[9] = float[](
//     1.0 / 16, 2.0 / 16, 1.0 / 16,
//     2.0 / 16, 4.0 / 16, 2.0 / 16,
//     1.0 / 16, 2.0 / 16, 1.0 / 16
//     );

// Edge detection:

// float kernel[9] = float[](
//     1.0,  1.0, 1.0,
//     1.0, -8.0, 1.0,
//     1.0,  1.0, 1.0
//     );

// Anti-Aliasing

// Saw-like jagged edges appear because of how the rasterizer transforms vertex data into fragments behind the scenes.
// This effect, of clearly seeing the pixels that compose an edge, is called aliasing.

// One technique to solve this problem is called Super-Sample Anti-Aliasing (SSAA)
// This technique temporarily uses a high resolution to render the scene (super-sampling), and it down-samples the resolution when
// it is time to update the visual output in the framebuffer. The extra resolution is used to eliminate the saw-like jagged edges.
// The only problem is that this technique comes with a major performance drawback, since it requires us to draw a lot more fragments
// than we normally would.

// SSAA gave birth to a more modern technique called Multisample Anti-Aliasing (MSAA), which implements a much more efficient approach.

// Multisampling
// +++++++++++++

// The rasterizer is the combination of all the algorithms and processes that sit between your final processed vertices
// and the fragment shader. The rasterizer takes all the vertices that belong to a single primitive and transforms the primitive
// to a set of fragments. Vertex coordinates can theoretically be any coordinate, but fragments can't since they are limited
// by the resolution of your window. There will almost never be a one-to-one mapping between vertex coordinates and fragments,
// so the rasterizer has to determine in some way at what fragment/screen-coordinate each specific vertex will end up at.

// Sample points are used to determine if a pixel is covered by a primitive or not.
// Due to the limited number of pixels in a screen, some pixels will be rendered along an edge and some won't.
// The result is that we're rendering primitives with non-smooth edges, giving rise to the jagged saw-like edges we've seen before.

// What multisampling does is use multiple sampling points to determine if a pixel is covered by a triangle.
// So instead of using a single sample point at the center of each pixel, it places 4 subsamples in a general pattern and uses them
// to determine pixel coverage.
// This means that the size of the color buffer is increased by the number of subsamples we're using per pixel.

// The number of sample points per pixel can be any number we like, with more samples giving us better coverage precision.

// Let's say we are using 4 sample points per pixel, and that we have a pixel with 2 of its 4 samples covered by a triangle.
// The next step is to determine a color for this specific pixel.
// Our initial guess would be that we run the fragment shader for each covered subsample and later average the colors of each subsample
// per pixel. In this case we'd run the fragment shader twice on the interpolated vertex data at each subsample, and store
// the resulting color in those sample points. This is, however, not how things work, because this basically means we need to run
// a lot more fragment shaders than without multisampling, drastically reducing performance.

// How MSAA really works is that the fragment shader is only run once per pixel (for each primitive), regardless of how many subsamples
// the triangle covers. The fragment shader is run with the vertex data interpolated to the center of the pixel, and the resulting
// color is then stored inside each of the covered subsamples. Once the color buffer's subsamples are filled with all the colors
// of the primitives we've rendered, all these colors are then averaged per pixel resulting in a single color per pixel.
// Because only 2 of the 4 samples are covered in our example, the color of the pixel is averaged with the triangle's color
// and the color stored at the other 2 sample points (in this case the clear color), resulting in a light blue-ish color.

// The result is a color buffer where all the primitive edges now produce a smoother pattern.

// Not only color values are affected by multisampling. The depth and stencil tests now make use of the multiple sample points.
// For depth testing, a vertex's depth value is interpolated to each subsample before running the depth test, and for stencil testing
// we store stencil values per subsample, instead of per pixel. This does mean that the size of the depth and stencil buffers
// are now also increased by the number of subsamples per pixel.

// MSAA in OpenGL
// +++++++++++++

// If we want to use MSAA in OpenGL we need to use a color buffer that is able to store more than one color value per pixel
// (since multisampling requires us to store a color per sample point). We thus need a new type of buffer called a multisample buffer.

// Most windowing systems are able to provide us with a multisample buffer instead of a normal color buffer.
// GLFW also gives us this functionality and all we need to do is hint to GLFW that we'd like to use a multisample buffer with
// N samples instead of a normal color buffer by calling glfwWindowHint before creating the window:

// glfwWindowHint(GLFW_SAMPLES, 4);

// When we now call glfwCreateWindow the rendering window is created, this time with a color buffer containing 4 subsamples
// per screen coordinate. GLFW also automatically creates a depth and stencil buffer with 4 subsamples per pixel.
// This does mean that the size of all the buffers is increased by 4.

// Now that we asked GLFW for multisampled buffers we need to enable multisampling by calling glEnable and enabling GL_MULTISAMPLE.
// On most OpenGL drivers, multisampling is enabled by default so this call is then a bit redundant, but it's usually a good idea
// to enable it anyways. This way all OpenGL implementations have multisampling enabled.

// glEnable(GL_MULTISAMPLE);

// Now the default framebuffer has multisampled buffer attachments, so we are done.
// The multisampling algorithms are implemented in the rasterizer of our OpenGL drivers.

// Off-screen MSAA
// +++++++++++++++

// Because GLFW takes care of creating the multisampled buffers, enabling MSAA is quite easy.
// If we want to use our own framebuffers however, for some off-screen rendering, we have to generate the
// multisampled buffers ourselves.

// Multisampled texture attachments
// ++++++++++++++++++++++++++++++++

// To create a texture that supports storage of multiple sample points we use glTexImage2DMultisample instead of glTexImage2D.
// glTexImage2DMultisample accepts GL_TEXTURE_2D_MULTISAPLE as its texture target:

// glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);

   // Generates a multisampled texture image on the currently bound texture object
   // This function differs from glTexImage2D in that the resulting texture will mainly be used as a multisampled buffer,
   // so we no longer provide the image data, the format of the image, or mipmap levels.
// glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, // target:               specifies the texture target. Examples:
                                                      //                       GL_TEXTURE_2D_MULTISAMPLE or GL_PROXY_TEXTURE_2D_MULTISAMPLE
//                         samples,                   // samples:              specifies the number of samples we want
//                         GL_RGB,                    // internalformat:       specifies the internal format to be used to store the
                                                      //                       multisample texture's image. It must specify a color-renderable,
                                                      //                       depth-renderable, or stencil-renderable format.
//                         width,                     // width:                width of the multisample texture's image, in texels.
//                         height,                    // height                height of the multisample texture's image, in texels.
//                         GL_TRUE);                  // fixedsamplelocations: specifies wether the image will use identical sample
                                                      //                       locations and the same number of samples for all texels
                                                      //                       in the image.

// glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

// glFramebufferTexture2D(GL_FRAMEBUFFER,            // target:     framebuffer type we are targeting (R, W, RW)
//                        GL_COLOR_ATTACHMENT0,      // attachment: type of attachment we are attaching. Options:
                                                     //             GL_COLOR_ATTACHMENTi
                                                     //             GL_DEPTH_ATTACHMENT
                                                     //             GL_STENCIL_ATTACHMENT
                                                     //             GL_DEPTH_STENCIL_ATTACHMENT
//                        GL_TEXTURE_2D_MULTISAMPLE, // textarget:  type of texture you want to attach.
//                        tex,                       // texture     texture to attach.
//                        0);                        // level:      mipmap level.

// To attach a multisampled texture to a framebuffer we use glFramebufferTexture2D, but this time with
// GL_TEXTURE_2D_MULTISAMPLE as the texture type:

// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0); 

// The currently bound framebuffer now has a multisampled color buffer in the form of a texture image.

// Multisampled renderbuffer objects
// +++++++++++++++++++++++++++++++++

// Like textures, creating a multisampled renderbuffer object isn't difficult. All we need to change is
// the call to glRenderbufferStorage to glRenderbufferStorageMultisample when we specify the memory storage of the currently
// bound renderbuffer object:

   // The function glRenderbufferStorageMultisample establishes data storage for a renderbuffer object.
   // When allocating memory, a multisampled renderbuffer is created storing memory equal to the
   // normal buffer's size times the number of samples.
// glRenderbufferStorageMultisample(GL_RENDERBUFFER,     // target: the RBO that is currently bound to this target is configured
//                                  4,                   // samples:        num of samples to be used for the RBO's storage
//                                  GL_DEPTH24_STENCIL8, // internalformat: internal format to use for the RBO's image
//                                  width,               // width:          width in pixels
//                                  height);             // height:         height in pixels

// The one thing that changed here is the extra parameter after the renderbuffer target where we set the
// amount of samples we'd like to use which is 4 in this particular case.

// Rendering to a multisampled framebuffer
// +++++++++++++++++++++++++++++++++++++++

// Rendering to a multisampled framebuffer happens automatically. Whenever we draw anything while the framebuffer object is bound,
// the rasterizer takes care of all the multisample operations.

// We then end up with a multisampled color/depth/stencil buffer. Because a multisampled buffer is different from a normal buffer,
// we can't sample its buffer image directly in a shader.

// A multisampled image contains much more information than a normal image, so what we need to do is downscale or resolve the image.
// Resolving a multisampled framebuffer is generally done via the glBlitFramebuffer function, which copies a region from one
// framebuffer to the other while also resolving any multisampled buffers.

// glBlitFramebuffer copies a block of pixels from the read framebuffer to the draw framebuffer.

// glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, // Bounds of the src rectangle within the read buffer of the
                                                                         // read framebuffer
//                   GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, // Bounds of the dst rectangle within the write buffer of the
                                                                         // write framebuffer
//                   GLbitfield mask,                                    // Bitwise OR of the flags that indicate which buffers are to
                                                                         // be copied. The allowed flags are:
                                                                         // GL_COLOR_BUFFER_BIT
                                                                         // GL_DEPTH_BUFFER_BIT
                                                                         // GL_STENCIL_BUFFER_BIT
//                   GLenum filter)                                      // Interpolation to be applied if the image is stretched.
                                                                         // Must be GL_NEAREST or GL_LINEAR

// Remember that if we bind to GL_FRAMEBUFFER we're binding to both the read and the draw framebuffer targets.
// We could also bind to those targets individually by binding the framebuffers to GL_READ_FRAMEBUFFER and GL_DRAW_FRAMEBUFFER.
// glBlitFramebuffer reads from those two targets to determine which is the source and which is the destination framebuffer.
// We could then transfer the multisampled framebuffer output to the screen by blitting the image to the default framebuffer:

// glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFBO);
// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
// glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST); 

// But what if we wanted to use the texture result of a multisampled framebuffer to do stuff like post-processing?
// We can't directly use the multisampled texture(s) in the fragment shader.
// What we can do is blit the multisampled buffer(s) to a different FBO with a non-multisampled texture attachment, and use this
// ordinary color attachment texture for post-processing, effectively post-processing an image rendered via multisampling.
// This does mean we have to generate a new FBO that acts solely as an intermediate framebuffer object to resolve the multisampled buffer
// into a normal 2D texture we can use in the fragment shader.
// This process looks like this:

   // Create FBO with multisampled attachments
// unsigned int framebuffer;
// glGenFramebuffers(1, &framebuffer);
// glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

   // Create a multisampled color attachment texture
// unsigned int textureColorBufferMultiSampled;
// glGenTextures(1, &textureColorBufferMultiSampled);
// glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
// glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
// glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

   // Create a multisampled renderbuffer object for depth and stencil attachments
// unsigned int rbo;
// glGenRenderbuffers(1, &rbo);
// glBindRenderbuffer(GL_RENDERBUFFER, rbo);
// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
// glBindRenderbuffer(GL_RENDERBUFFER, 0);
// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

// if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
// cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
// glBindFramebuffer(GL_FRAMEBUFFER, 0);

   // Configure second post-processing framebuffer
// unsigned int intermediateFBO;
// glGenFramebuffers(1, &intermediateFBO);
// glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

   // Create a color attachment texture
// unsigned int screenTexture;
// glGenTextures(1, &screenTexture);
// glBindTexture(GL_TEXTURE_2D, screenTexture);
// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0); // We only need a color buffer

// if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
// cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
// glBindFramebuffer(GL_FRAMEBUFFER, 0);

// while(!glfwWindowShouldClose(window))
// {
//     ...

       // clear the default framebuffer's content
//     glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       // 1) draw scene as normal in multisampled buffers
//     glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//     glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     glEnable(GL_DEPTH_TEST);

//     ...

       // 2) now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
//     glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
//     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
//     glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

       // 3) now render quad with scene's visuals as its texture image into the default framebuffer
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT);
//     glDisable(GL_DEPTH_TEST);

//     ...
// }

// After this process, because the screen texture is a normal texture again with just a single sample point, some post-processing
// filters like edge-detection will introduce jagged edges again. To accommodate for this, you could blur the texture afterwards or
// create your own anti-aliasing algorithm.

// Custom Anti-Aliasing algorithm
// ++++++++++++++++++++++++++++++

// It is also possible to directly pass a multisampled texture image to the shaders instead of first resolving them.
// GLSL then gives us the option to sample the texture images per subsample so we can create our own anti-aliasing algorithms,
// which is commonly done by large graphics applications.

// To retrieve the color value per subsample you'd have to define the texture uniform sampler as a sampler2DMS
// instead of the usual sampler2D:

// uniform sampler2DMS screenTextureMS;

// Using the texelFetch function it is then possible to retrieve the color value per sample:

// vec4 colorSample = texelFetch(screenTextureMS, TexCoords, 3); // 4th subsample

// Text Rendering

// There are no graphical primitives for text characters in OpenGL
// Some techniques for rendering text are:

// - Drawing letter shapes via GL_LINES
// - Creating 3D meshes of letters
// - Rendering character textures to 2D quads in a 3D environment

// The last one is the most popular.

// In the early days of computer graphics, rendering text involved selecting a font and extracting all of its characters by
// pasting them within a single large texture. Such a texture, known as a bitmap font, would then contain all the character symbols
// in predefined regions of its surface.
// These character symbols of the font are known as glyphs. Each glyph has a specific region of texture coordinates associated with it.
// Whenever you want to render a character, you select the corresponding glyph by rendering its section of the bitmap font to a 2D quad.

// This approach has several advantages and disadvantages.
// Advantages:
// - Easy to implement
// - Because bitmap fonts are pre-rasterized, they are quite efficient.
// Disadvantages:
// - Unflexible.
// - When you want to use a different font, you need to recompile a new bitmap font,
// - The system is limited to a single resolution. Zooming in will show pixelated edges.
// - Often limited to a small character set so Extended or Unicode characters are often unavailable.

// More flexible approach: loading TrueType fonts using the FreeType library.

// FreeType is library that is able to load fonts, render them to bitmaps and provide support for several font-related operations.
// A TrueType font is a collection of character glyphs that are not defined by pixels or any other non-scalable parameters,
// but instead by mathematical equations (combinations of splines).
// Similar to vector images, the rasterized font images can be procedurally generated based on the font height you desire.
// By using TrueType fonts you can easily render character glyphs of various sizes without any loss of quality.

// What FreeType does is load a TrueType font, generate a bitmap image for each glyph and calculate several metrics.
// We can extract these bitmap images for generating textures and we can position each character glyph
// appropriately using the loaded metrics.

// To load a font, all we have to do is initialize the FreeType library and load the font as a face.
// Here we load the arial.ttf TrueType font file, which can be found in the Windows/Fonts directory.
// Each of these FreeType functions returns a non-zero integer whenever an error occurred.

// FT_Library ft;
// if (FT_Init_FreeType(&ft))
//     std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

// FT_Face face;
// if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
//     std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

// Once we've loaded the face, we should define the font size we'd like to extract from this face.
// The function below sets the font's width and height parameters. Setting the width to 0 lets the face dynamically calculate
// the width based on the given height.

// FT_Set_Pixel_Sizes(face, 0, 48);

// A FreeType face hosts a collection of glyphs. We can set one of those glyphs as the active glyph by calling FT_Load_Char.
// Here we choose to load the character glyph 'X':

// if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
//     std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

// By setting FT_LOAD_RENDER as one of the loading flags, we tell FreeType to create an 8-bit grayscale bitmap image
// that we can access via face->glyph->bitmap.

// The glyphs we load with FreeType do not have the same size, as was the case with bitmap fonts.
// The bitmap image generated by FreeType is just large enough to contain the visible part of a character.
// For example, the bitmap image of the dot character '.' is much smaller than the bitmap image of the character 'X'.
// For this reason, FreeType also loads several metrics that specify how large each character should be and how to
// properly position them.

// Each of the glyphs reside on a horizontal baseline.
// Some sit exactly on top of this baseline (like 'X'), while others sit slightly below it (like 'g' or 'p').
// These metrics define:
// - The necessary offsets to properly position each glyph on the baseline.
// - How large each glyph should be.
// - How many pixels we need to advance to render the next glyph.

// Below is a small list of these metrics that are relevant to us:

// width:    The width (in pixels) of the bitmap.
//           This value is accessed via face->glyph->bitmap.width
// height:   The height (in pixels) of the bitmap.
//           This value is accessed via face->glyph->bitmap.rows.
// bearingX: The horizontal bearing e.g. the horizontal position (in pixels) of the bitmap relative to the origin.
//           This value is accessed via face->glyph->bitmap_left.
// bearingY: The vertical bearing e.g. the vertical position (in pixels) of the bitmap relative to the baseline.
//           This value is accessed via face->glyph->bitmap_top.
// advance:  The horizontal advance e.g. the horizontal distance (in 1/64th pixels) from the origin to the origin of the next glyph.
//           This value is accessed via face->glyph->advance.x.

// We could load a character glyph, retrieve its metrics and generate a texture each time we want to render it to the screen,
// but it would be inefficient to do this for each frame.
// So instead, we will store the generated data somewhere in the application and query it whenever we want to render a character.
// To do this we will need the following struct and map:

// struct Character {
//     GLuint     TextureID;  // ID handle of the glyph texture
//     glm::ivec2 Size;       // Size of glyph
//     glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
//     GLuint     Advance;    // Offset to advance to next glyph
// };

// std::map<GLchar, Character> Characters;

// For this tutorial we'll keep things simple by restricting ourselves to the first 128 characters of the ASCII character set.
// For each character, we generate a texture and store its relevant data into a Character struct that we then add to the Characters map.
// This way, all data required to render each character is stored for later use.


// glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

// for (GLubyte c = 0; c < 128; c++)
// {
//     // Load character glyph 
//     if (FT_Load_Char(face, c, FT_LOAD_RENDER))
//     {
//         std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
//         continue;
//     }

       // Generate texture
//     GLuint texture;
//     glGenTextures(1, &texture);
//     glBindTexture(GL_TEXTURE_2D, texture);
//     glTexImage2D(GL_TEXTURE_2D,
//                  0,
//                  GL_RED,
//                  face->glyph->bitmap.width,
//                  face->glyph->bitmap.rows,
//                  0,
//                  GL_RED,
//                  GL_UNSIGNED_BYTE,
//                  face->glyph->bitmap.buffer);

       // Set texture options
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

       // Now store character for later use
//     Character character = { texture,
//                             glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
//                             glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
//                             face->glyph->advance.x };

//     Characters.insert(std::pair<GLchar, Character>(c, character));
// }

// What is interesting to note here is that we use GL_RED as the texture's internalFormat and format arguments.
// The bitmap generated from the glyph is a grayscale 8-bit image where each color is represented by a single byte.
// For this reason we'd like to store each byte of the bitmap buffer as a texture's color value.
// We accomplish this by creating a texture where each byte corresponds to the texture color's red component
// (first byte of its color vector). If we use a single byte to represent the colors of a texture we do need to take care
// of a restriction of OpenGL:

// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

// OpenGL requires that textures have a 4-byte alignment e.g. that their size is always a multiple of 4 bytes.
// Normally this won't be a problem since most textures have a width that is a multiple of 4 and/or use 4 bytes per pixel,
// but since we now only use a single byte per pixel they can have any possible width.
// By setting the unpack alignment equal to 1 we ensure there won't be any alignment issues (which could cause segmentation faults).

// Also be sure to clear FreeType's resources once you're finished processing the glyphs:

// FT_Done_Face(face);
// FT_Done_FreeType(ft);

// To render the glyphs we'll be using the following vertex shader:

// #version 330 core
//
// layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
//
// out vec2 TexCoords;
//
// uniform mat4 projection;
//
// void main()
// {
//     gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
//     TexCoords = vertex.zw;
// }

// And the following fragment shader:

// #version 330 core
//
// in vec2 TexCoords;
//
// out vec4 color;
//
// uniform sampler2D text;
// uniform vec3 textColor;
//
// void main()
// {
//     vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
//     color = vec4(textColor, 1.0) * sampled;
// }

// The fragment shader takes two uniforms:
// - The mono-colored bitmap image of the glyph.
// - A color uniform for adjusting the text's final color.
// We first sample the color value of the bitmap texture. Because the texture's data is stored in just its red component,
// we sample the r component of the texture as the sampled alpha value. By varying the color's alpha value the resulting color
// will be transparent for all the glyph's background colors and non-transparent for the actual character pixels.
// We also multiply the RGB colors by the textColor uniform to vary the text color.
// We do need to enable blending for this to work though:

// glEnable(GL_BLEND);
// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// For the projection matrix we'll be using an orthographic projection matrix.
// For rendering text we (usually) do not need perspective and using an orthographic projection matrix also allows us
// to specify all the vertex coordinates in screen coordinates if we set it up as follows:

// glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

// We set the projection matrix's bottom parameter to 0.0f and its top parameter equal to the window's height.
// The result is that we can specify coordinates with y values ranging from the bottom part of the screen (0.0f)
// to the top part of the screen (600.0f). This means that the point (0.0, 0.0) now corresponds to the bottom-left corner.

// Finally we create a VBO and VAO for rendering the quads. For now we reserve enough memory when initializing the VBO so that
// we can later update the VBO's memory when rendering characters.

// GLuint VAO, VBO;
// glGenVertexArrays(1, &VAO);
// glGenBuffers(1, &VBO);
// glBindVertexArray(VAO);
// glBindBuffer(GL_ARRAY_BUFFER, VBO);
// glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
// glEnableVertexAttribArray(0);
// glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
// glBindBuffer(GL_ARRAY_BUFFER, 0);
// glBindVertexArray(0);

// The 2D quad requires 6 vertices of 4 floats each (2 floats for position and 2 for tex coords) so we reserve 6 * 4 floats of memory.
// Because we'll be updating the content of the VBO's memory quite often we'll allocate the memory with GL_DYNAMIC_DRAW.

// To render a character we extract the corresponding Character struct of the Characters map and calculate the quad's dimensions
// using the character's metrics. With the quad's calculated dimensions we dynamically generate a set of 6 vertices
// that we use to update the content of the memory managed by the VBO using glBufferSubData.

// void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
// {
       // Activate corresponding render state
//     s.Use();
//     glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);
//     glActiveTexture(GL_TEXTURE0);
//     glBindVertexArray(VAO);

       // Iterate through all characters
//     std::string::const_iterator c;
//     for (c = text.begin(); c != text.end(); c++)
//     {
//         Character ch = Characters[*c];

           // See Text Rendering tutorial to understand these calculations
           // xpos and ypos correspond to the lower left corner of the glyph
//         GLfloat xpos = x + ch.Bearing.x * scale;
//         GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

//         GLfloat w = ch.Size.x * scale;
//         GLfloat h = ch.Size.y * scale;

           // Update VBO for each character
//         GLfloat vertices[6][4] = {
//             { xpos,     ypos + h,   0.0, 0.0 }, // Top left
//             { xpos,     ypos,       0.0, 1.0 }, // Bottom left
//             { xpos + w, ypos,       1.0, 1.0 }, // Bottom right
// 
//             { xpos,     ypos + h,   0.0, 0.0 }, // Top left
//             { xpos + w, ypos,       1.0, 1.0 }, // Bottom right
//             { xpos + w, ypos + h,   1.0, 0.0 }  // Top right
//         };

           // Render glyph texture over quad
//         glBindTexture(GL_TEXTURE_2D, ch.textureID);

           // Update content of VBO memory
//         glBindBuffer(GL_ARRAY_BUFFER, VBO);
//         glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);

           // Render quad
//         glDrawArrays(GL_TRIANGLES, 0, 6);

           // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//         x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
//     }

//     glBindVertexArray(0);
//     glBindTexture(GL_TEXTURE_2D, 0);
// }

// If you did everything correct you should now be able to successfully render strings of text with the following statements:

// RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
// RenderText(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

// This approach is likely going to be overkill for your application as we generate and render textures for each glyph.
// Performance-wise, bitmap fonts are preferable as we only need one texture for all our glyphs.
// The best solution would be to combine the two approaches by dynamically generating a bitmap font texture that features all the
// characters glyphs that were loaded with FreeType. This saves the renderer from a significant amount of texture switches
// and based on how tightly each glyph is packed, it could improve performance quite a bit.

// Cubemaps

// A cubemap is a texture that contains 6 individual 2D textures that each form one side of a cube.
// Cubemaps have the useful property that they can be indexed/sampled using a direction vector.
// If we imagine we have a cube shape that we attach a cubemap to, the direction vectors used to sample the cubemap
// would be similar to the (interpolated) vertex positions of the cube.
// This way we can sample the cubemap using the cube's actual position vectors as long as the cube is centered on the origin.
// We can then retrieve the texture coordinates of all the vertices as the vertex positions of the cube.

// To create a cubemap:

// unsigned int textureID;
// glGenTextures(1, &textureID);
// glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

// Because a cubemap consists of 6 textures, one for each face, we have to call glTexImage2D six times.
// This time however, we have to set the texture target parameter of each call to a specific face of the cubemap,
// basically telling OpenGL which side of the cubemap we're creating a texture for.
// These are the 6 special texture targets:

// GL_TEXTURE_CUBE_MAP_POSITIVE_X - Right
// GL_TEXTURE_CUBE_MAP_NEGATIVE_X - Left
// GL_TEXTURE_CUBE_MAP_POSITIVE_Y - Top
// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y - Bottom
// GL_TEXTURE_CUBE_MAP_POSITIVE_Z - Back
// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z - Front

// Like many of OpenGL's enums, their behind-the-scenes int value is linearly incremented so if we were to have an array or vector
// of texture locations, we could loop over them by starting with GL_TEXTURE_CUBE_MAP_POSITIVE_X and incrementing the enum by 1
// each iteration, effectively looping through all the texture targets:

// int width, height, nrChannels;
// unsigned char *data;
// for (GLuint i = 0; i < textures_faces.size(); i++)
// {
//     data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
//     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//                  0,
//                  GL_RGB,
//                  width,
//                  height,
//                  0,
//                  GL_RGB,
//                  GL_UNSIGNED_BYTE,
//                  data);
// }

// Here we have a vector called textures_faces that contains the locations of all the textures required for the cubemap
// in the order of the table above.

// Because a cubemap is a texture like any other texture we will also specify its wrapping and filtering methods:

// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

// GL_TEXTURE_WRAP_R simply sets the wrapping method for the texture's R coordinate which corresponds to the texture's 3rd dimension
// (like the z for positions). We set the wrapping method to GL_CLAMP_TO_EDGE since texture coordinates that are exactly between
// two faces might not hit an exact face (due to some hardware limitations), so by using GL_CLAMP_TO_EDGE OpenGL always returns
// their edge values whenever we sample between faces.

// Then before drawing the objects that will use the cubemap, we activate the corresponding texture unit and bind the cubemap.

// Within the fragment shader we also have to use a different sampler of the type samplerCube, which we can still sample using
// the texture function, but this time using a vec3 direction vector instead of a vec2.

// in vec3 textureDir; // direction vector representing a 3D texture coordinate
// uniform samplerCube cubemap; // cubemap texture sampler
//
// void main()
// {
//     FragColor = texture(cubemap, textureDir);
// }

// Skybox

// A skybox is a (large) cube that encompasses the entire scene and that contains 6 images of a surrounding environment.

// Loading a skybox

// unsigned int loadCubemap(vector<std::string> faces)
// {
//     unsigned int textureID;
//     glGenTextures(1, &textureID);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
// 
//     int width, height, nrChannels;
//     for (unsigned int i = 0; i < faces.size(); i++)
//     {
//         unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//         if (data)
//         {
//             glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//             stbi_image_free(data);
//         }
//         else
//         {
//             std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
//             stbi_image_free(data);
//         }
//     }
//
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
// 
//     return textureID;
// }

// Before we call this function we need load the appropriate texture paths in a vector in the order specified by the cubemap enums:

// vector<std::string> faces;
// {
//     "right.jpg",
//     "left.jpg",
//     "top.jpg",
//     "bottom.jpg",
//     "front.jpg",
//     "back.jpg"
// };
// 
// unsigned int cubemapTexture = loadCubemap(faces); 

// Displaying a skybox

// Because a skybox is drawn on a cube we'll need another VAO, VBO and a set of vertices like for any other object.

// A cubemap used to texture a 3D cube can be sampled using the positions of the cube as the texture coordinates.
// When a cube is centered on the origin, each of its position vectors is also a direction vector from the origin.
// These direction vectors are exactly what we need to get their corresponding texture values at their specific positions on the cube.
// For this reason we only need to supply position vectors and no texture coordinates.

// This is the vertex shader we need to render a skybox:

// #version 330 core
// layout (location = 0) in vec3 aPos;
// 
// out vec3 TexCoords;
// 
// uniform mat4 projection;
// uniform mat4 view;
// 
// void main()
// {
//     TexCoords = aPos;
//     gl_Position = projection * view * vec4(aPos, 1.0);
// }

// And this is the fragment shader that we need to render a skybox:

// #version 330 core
// out vec4 FragColor;
// 
// in vec3 TexCoords;
// 
// uniform samplerCube skybox;
// 
// void main()
// {
//     FragColor = texture(skybox, TexCoords);
// }

// Rendering the skybox is easy now that we have a cubemap texture. We simply bind the cubemap texture and the skybox sampler
// is automatically filled with the skybox cubemap. To draw the skybox we're going to disable depth writing and we are going to
// draw it as the first object in the scene. This way the skybox will always be drawn as the background of all the other objects.

// glDepthMask(GL_FALSE);
// skyboxShader.use();
// ... set view and projection matrix
// glBindVertexArray(skyboxVAO);
// glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
// glDrawArrays(GL_TRIANGLES, 0, 36);
// glDepthMask(GL_TRUE);
// ... draw rest of the scene

// If you run this you will get into difficulties though. We want the skybox to be centered around the player so that no matter
// how far the player moves, the skybox won't get any closer, giving the impression that the surrounding environment is extremely large.
// The current view matrix transforms all the skybox's positions by rotating, scaling and translating them, so if the player moves,
// the cubemap moves as well. We want to remove the translation part of the view matrix so that movement doesn't affect
// the skybox's position vectors.

// We can remove the translation part of a transformation matrix by taking the upper-left 3x3 matrix of the 4x4 matrix.

// glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

// This removes any translation, but keeps all the rotation so that the user can still look around the scene.
// Remember that a view matrix is the inverse of the translation and rotation that we would apply to the camera to position it
// The translation part of the view matrix pushes back the world. So if we remove the translation part, the center of the skybox
// is always at the location of the camera

// Optimizing skybox rendering

// We have rendered the skybox before any of the other objects in the scene. This works great, but it isn't too efficient.
// If we render the skybox first we're running the fragment shader for each pixel on the screen even though only a small part
// of the skybox will eventually be visible. So we are rendering fragments that could have easily been discarded using
// early depth testing.

// So to give us a slight performance boost we're going to render the skybox last. This way, the depth buffer is completely filled
// with all the other objects' depth values by the time we get to the skybox, so we only have to render the skybox's fragments
// wherever the early depth test passes, which greatly reduces the number of calls to the fragment shader.

// There is a problem though:
// Imagine the skybox as a 1x1x1 cube whose center is always at the postion of the camera. This cube acts as a shell that prevents
// you from the seeing the other objects in the scene, unless you get close enough to them so that they fall within the 1x1x1 range.

// Simply rendering the skybox without depth testing is not a solution since the skybox will then overwrite all the other objects in the scene.
// For this reason, we need to trick the depth buffer into believing that the skybox has the maximum depth value of 1.0,
// so that it fails the depth test wherever there's a different object in front of it.

// We know that perspective division is performed after the vertex shader has run, dividing the gl_Position's xyz coordinates by its
// w component. We also know that the z component of the resulting division is equal to the vertex's depth value.
// This means that we can set the z component of gl_Position equal to its w component, which will result in a z component
// that is always equal to 1.0, because when the perspective division is performed its z component translates to w / w = 1.0

// void main()
// {
//     TexCoords = aPos;
//     vec4 pos = projection * view * vec4(aPos, 1.0);
//     gl_Position = pos.xyww;
// }

// The resulting normalized device coordinates will then always have a z value equal to 1.0: the maximum depth value.
// The skybox will as a result only be rendered wherever there are no objects visible (only then it will pass the depth test,
// everything else is in front of the skybox).

// We do have to change the depth function a little by setting it to GL_LEQUAL instead of the default GL_LESS.
// The depth buffer will be filled with values of 1.0 for the skybox, so we need to make sure the skybox passes the depth tests
// with values less than or equal to the depth buffer instead of less than.

// To achieve reflections with a skybox, use this fragment shader:

// #version 330 core
// out vec4 FragColor;
// 
// in vec3 Position;
// in vec3 Normal;
// 
// uniform vec3 cameraPos;
// uniform samplerCube skybox;
// 
// void main()
// {
//     vec3 I    = normalize(Position - cameraPos);
//     vec3 R    = reflect(I, normalize(Normal));
//     FragColor = vec4(texture(skybox, R).rgb, 1.0);
// }

// To achieve refractions with a skybox, use this fragment shader:

// #version 330 core
// out vec4 FragColor;
// 
// in vec3 Position;
// in vec3 Normal;
// 
// uniform vec3 cameraPos;
// uniform samplerCube skybox;
// 
// void main()
// {
//     float ratio = 1.00 / 1.52; // Refractive index of air divided by the refractive index of glass = N_src / N_dest
//     vec3 I    = FragPos - cameraPos;
//     vec3 R    = refract(I, normalize(Normal), ratio);
//     FragColor = vec4(texture(skybox, R).rgb, 1.0);
// }

// Dynamic Environment Maps

// So far we have used the skybox to create reflections and refractions. These look great, but only because we only have a single
// object in our scene. If we had a reflective nanosuit surrounded by many containers, only the skybox would be reflected by the
// nanosuit.

// Using framebuffers, it is possible to create a texture of the scene for all 6 different views (right, left, top, bottom, near, far)
// for a specific object, and store those in a cubemap in each render iteration.
// We can then use this dynamically generated cubemap to create realistic reflective and refractive surfaces that reflect and refract
// all the other objects in the scene. This is called Dynamic Environment Mapping, because we dynamically create a cubemap of an
// object's surroundings and use that as its environment map.

// While the results of DEM look great, this technique has one enormous disadvantage: we have to render the scene 6 times per object
// using an environment map, which is an enormous performance penalty. Modern applications try to use skyboxes as much as possible, and
// they also use pre-compiled cubemaps wherever they can to create Dynamic Environment Maps.

// Advanced Data

// A buffer in OpenGL is only an object that manages a certain piece of memory and nothing more.
// We give a meaning to a buffer when binding it to a specific buffer target. A buffer is only a vertex array buffer
// when we bind it to GL_ARRAY_BUFFER, but we could just as easily bind it to GL_ELEMENT_ARRAY_BUFFER. OpenGL internally stores
// a buffer per target and based on the target, processes the buffers differently.

// So far we've been filling the memory managed by the buffer objects by calling glBufferData, which allocates a piece of memory
// and adds data into this memory. If we were to pass NULL as its data argument, the function would only allocate memory
// and not fill it. This is useful if we first want to reserve a specific amount of memory and later come back to this buffer
// to fill it piece by piece.

// Instead of filling the entire buffer with one function call we can also fill specific regions of the buffer by calling
// glBufferSubData. This function expects a buffer target, an offset, the size of the data and the actual data as its arguments.
// What's new with this function is that we can now give an offset that specifies from where we want to fill the buffer.
// This allows us to insert/update only certain parts of the buffer's memory. Do note that the buffer should have enough allocated
// memory, so a call to glBufferData is necessary before calling glBufferSubData on the buffer.

// glBufferSubData(GL_ARRAY_BUFFER, // Target
//                 24,              // Offset in bytes
//                 sizeof(data),    // Size of data to write in bytes
//                 &data);          // Data to write

// Yet another method for getting data into a buffer is to ask for a pointer to the buffer's memory and directly copy the data
// to the buffer by yourself. By calling glMapBuffer OpenGL returns a pointer to the currently bound buffer's memory
// for us to operate on:

// float data[] = {
//     0.5f, 1.0f, -0.35f
//     ...
// };
//
// glBindBuffer(GL_ARRAY_BUFFER, buffer);
//
   // get pointer
// void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY); // Options for the access value:
                                                            // GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE
//
   // now copy data into memory
// memcpy(ptr, data, sizeof(data));
   // make sure to tell OpenGL we're done with the pointer
// glUnmapBuffer(GL_ARRAY_BUFFER);

// By telling OpenGL we're finished with the pointer operations via glUnmapBuffer OpenGL knows you're done. By unmapping,
// the pointer becomes invalid and the function returns GL_TRUE if OpenGL was able to map your data successfully to the buffer.

// Using glMapBuffer is useful to directly map data to a buffer, without first storing it in temporary memory.
// Think of directly reading data from a file and copying it into a buffer's memory.

// Batching vertex attributes

// Using glVertexAttribPointer we were able to specify the attribute layout of a VAO's content. Within the VAO we interleaved
// the attributes; that is, we placed the position, normal and/or texture coordinates next to each other for each vertex.
// Now that we know a bit more about buffers we could take a different approach.

// What we could do instead is batch all the vector data into large chunks per attribute type instead of interleaving them.
// So instead of an interleaved layout 123123123123 we take a batched approach 111122223333.

// When loading vertex data from a file you generally retrieve an array of positions, an array of normals and/or an array
// of texture coordinates. It might cost some effort to combine these arrays into one large array of interleaved data.
// Taking the batching approach is then an easier solution that we can easily implement using glBufferSubData:

// float positions[] = { ... };
// float normals[] = { ... };
// float tex[] = { ... };
   // fill buffer
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions);
// glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(normals), &normals);
// glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(normals), sizeof(tex), &tex);

// This way we can directly transfer the attribute arrays as a whole into the buffer without first having to process them.
// We could have also combined them into one large array and filled the buffer right away using glBufferData, but using
// glBufferSubData lends itself perfectly for tasks like these.

// We'll also have to update the vertex attribute pointers to reflect these changes:

// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  
// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(positions)));  
// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(positions) + sizeof(normals)));

// Copying buffers

// Once a buffer is filled with your data you could want to share that data with other buffers, or perhaps copy the buffer's
// content into another buffer. The function glCopyBufferSubData allows us to copy data from one buffer to another buffer.
// The function's prototype is as follows:

//void glCopyBufferSubData(GLenum readtarget,
//                         GLenum writetarget,
//                         GLintptr readoffset,
//                         GLintptr writeoffset,
//                         GLsizeiptr size);

// We could for example copy from a VERTEX_ARRAY_BUFFER buffer to a VERTEX_ELEMENT_ARRAY_BUFFER buffer by specifying those
// buffer targets as the read and write targets respectively. The buffers currently bound to those buffer targets will then
// be affected.

// But what if we wanted to read and write data into two different buffers that are both vertex array buffers?
// We can't bind two buffers at the same time to the same buffer target. For this reason, and this reason alone, OpenGL gives us
// two more buffer targets called GL_COPY_READ_BUFFER and GL_COPY_WRITE_BUFFER. We can then bind the buffers of our choice to
// these new buffer targets and set those targets as the readtarget and writetarget arguments.

// glCopyBufferSubData then reads data of a given size from a given readoffset and writes it into the writetarget buffer at
// writeoffset. An example of copying the content of two vertex array buffers is shown below:

// float vertexData[] = { ... };
// glBindBuffer(GL_COPY_READ_BUFFER, vbo1);
// glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
// glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(vertexData));

// We could've also done this by only binding the writetarget buffer to one of the new buffer target types:

// float vertexData[] = { ... };
// glBindBuffer(GL_ARRAY_BUFFER, vbo1);
// glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
// glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(vertexData));

// Advanced GLSL

// Vertex shader variables

// 1) glPosition: Clip-space output position vector of the vertex shader.
//                Setting glPosition is required to render anything to the screen.

// 2) gl_PointSize: Recall that using GL_POINTS with glDrawElements allows us to render each vertex primitive as a point.
//                  It is possible to set the size of the points being rendered via OpenGL's glPointSize function, but we can
//                  also influence this value in the vertex shader.
//                  The output float variable gl_PointSize allows us to set the width and height of the points in pixels.
//                  By describing the point's size in the vertex shader you can influence this point value per vertex.
//                  Influencing the point sizes in the vertex shader is disabled by default, but you can enable this functionality
//                  by enabling OpenGL's GL_PROGRAM_POINT_SIZE:

//                  glEnable(GL_PROGRAM_POINT_SIZE);

//                  A simple example of influencing the point sizes is by setting the point size equal to the
//                  clip-space position's z value which is equal to the vertex's distance to the viewer.
//                  The point size should then increase the further we are from the vertices as the viewer.

//                  void main()
//                  {
//                      gl_Position = projection * view * model * vec4(aPos, 1.0);
//                      gl_PointSize = gl_Position.z;
//                  }

//                  gl_PointSize is great for particle generation.

// 3) gl_VertexID: gl_Position and gl_PointSize are output variables, which means that we can influence
//                 the results of the vertex shader by writing to them.
//                 The vertex shader also gives us an interesting input variable, that we can only read from, called gl_VertexID.
//                 The integer input variable gl_VertexID holds the ID of the vertex we are currently drawing.
//                 When doing indexed rendering (with glDrawElements), this variable holds the index
//                 of the vertex we are currently drawing.
//                 When drawing without indices (via glDrawArrays), this variable holds the number of vertices
//                 that have been processed since the start of the render call.

// Fragment shader variables

// 1) gl_FragCoord: This vec3 input variable contains the window-space coordinates of the fragment we are currently processing,
//                  as well as its depth.
//                  It is particularly useful for depth testing because its Z component is equal to the depth of the fragment.
//                  The X and Y window-space coordinates originate from the bottom-left corner of the window.
//                  So if we specify a window of 800 x 600 pixels with glViewport, the X component will range from 0 to 800,
//                  and the Y component will range from 0 to 600.
//                  Using this variable we can perform different fragment shader calculations based
//                  on the window coordinates of each fragment.
//                  A common usage of the gl_FragCoord variable is for comparing the visual output of
//                  different fragment calculations, as usually seen in tech demos.
//                  We could for example split the screen in two by rendering one output to the left side of the window
//                  and another output to the right side of the window.

//                  void main()
//                  {
//                      if (gl_FragCoord.x < 400)
//                          FragColor = vec4(1.0, 0.0, 0.0, 1.0);
//                      else
//                          FragColor = vec4(0.0, 1.0, 0.0, 1.0);
//                  }

// 2) gl_FrontFacing: In the face culling tutorial we mentioned that OpenGL is able to figure out if a face is a front or back face
//                    based on the winding order of the vertices. If we're not using face culling (by enabling GL_FACE_CULL) then
//                    the gl_FrontFacing input variable tells us if the current fragment is part of a front-facing or a back-facing face.
//                    We could then decide to calculate different colors for front faces for example.

//                    The gl_FrontFacing variable is a bool that is true if the fragment is part of a front face.
//                    Using this variable we can create a cube with a different texture on the inside than on the outside:

//                    #version 330 core
//                    out vec4 FragColor;
//                    
//                    in vec2 TexCoords;
//                    
//                    uniform sampler2D frontTexture;
//                    uniform sampler2D backTexture;
//                    
//                    void main()
//                    {
//                        if(gl_FrontFacing)
//                            FragColor = texture(frontTexture, TexCoords);
//                        else
//                            FragColor = texture(backTexture, TexCoords);
//                    }

// 3) gl_FragDepth: The gl_FragCoord input variable allows us to read window-space coordinates and the depth value of the fragment
//                  that is currently being processed, but it is a read-only variable, so we can't use it to change the window-space
//                  coordinates of the fragment or its depth value.
//                  It is possible, however, to set the depth value of a fragment using an output variable called gl_FragDepth.
//                  To do this, we simply write a float value between 0.0 and 1.0 to this variable in the fragment shader:

//                  gl_FragDepth = 0.0; // this fragment now has a depth value of 0.0

//                  If the fragment shader does not write a value to gl_FragDepth, this variable will automatically take its value from gl_FragCoord.z.

//                  Modifying the depth value of a fragment through gl_FragDepth has a major disadvantage, however, because doing this causes OpenGL
//                  to disable early depth testing, which means that the fragment shader will be executed for all fragments, regardless of whether
//                  they are behind of other fragments or not. This occurs because OpenGL cannot know the depth value of a fragment before it runs
//                  the fragment shader, since the fragment shader might change the depth value of the fragment.

//                  Starting with OpenGL 4.2, however, we can still take advantage of early depth testing in certain situations by redeclaring
//                  the gl_FragDepth variable at the top of the fragment shader with a depth condition:

//                  layout (depth_<condition>) out float gl_FragDepth;

//                  This condition can take the following values:

//                  | any       | The default value. Early depth testing is disabled.                   |
//                  | greater   | You can only make the depth value larger compared to gl_FragCoord.z.  |
//                  | less      | You can only make the depth value smaller compared to gl_FragCoord.z. |
//                  | unchanged | If you write to gl_FragDepth, you will write exactly gl_FragCoord.z.  |

//                  Below is an example of a fragment shader that increments the depth value of fragments while preserving early depth testing:

//                  #version 420 core // Note the GLSL version!
//                  out vec4 FragColor;
//                  layout (depth_greater) out float gl_FragDepth;

//                  void main()
//                  {
//                      FragColor = vec4(1.0);
//                      gl_FragDepth = gl_FragCoord.z + 0.1;
//                  }

// Interface blocks

// So far, every time we wanted to send data from the vertex to the fragment shader we declared several matching input/output
// variables. Declaring these one at a time is the easiest way to send data from one shader to another, but as your applications
// become larger, you will probably want to send more than a few variables over, including arrays and/or structs.

// To help us organize these variables GLSL offers us something called interface blocks, which allow us to group together variables.
// The declaration of such an interface block looks a lot like a struct declaration, except that it is now declared using an
// in or out keyword based on whether the block is being inputted or outputted.

// #version 330 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoords;
//
// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;
//
// out VS_OUT
// {
//     vec2 TexCoords;
// } vs_out;
//
// void main()
// {
//     gl_Position = projection * view * model * vec4(aPos, 1.0);
//     vs_out.TexCoords = aTexCoords;
// }

// This is very useful when we want to group shader inputs and outputs into arrays (see the section on geometry shaders).

// Now that we have declared the output interface block, we must also declare the input interface block in the next shader,
// which in this case is the fragment shader. The block name (VS_OUT) should be the same in the fragment shader,
// but the instance name (vs_out as used in the vertex shader) can be anything we like.

// #version 330 core
// out vec4 FragColor;
//
// in VS_OUT
// {
//     vec2 TexCoords;
// } fs_in;
//
// uniform sampler2D texture;
//
// void main()
// {
//     FragColor = texture(texture, fs_in.TexCoords);
// }

// Uniform buffer objects

// When using more than one shader, we continually set uniform variables for each one, even when most of them are
// exactly the same for each shader.

// OpenGL gives us a tool called uniform buffer objects that allow us to declare a set of global uniform variables
// that remain the same over several shader programs. When using uniform buffer objects we thus have to set the relevant uniforms
// only once. We do still have to manually set the uniforms that are unique per shader, however.

// Because a uniform buffer object is a buffer like any other buffer, we can create one via glGenBuffers, bind it to the
// GL_UNIFORM_BUFFER buffer target, and store all the relevant uniform data into the buffer.
// Note that there are certain rules that define how the data for uniform buffer objects should be stored.

// Take the following vertex shader, for example, in which we store the projection and view matrices in a uniform block:

// #version 330 core
// layout (location = 0) in vec3 aPos;
//
// layout (std140) uniform Matrices
// {
//     mat4 projection;
//     mat4 view;
// };
//
// uniform mat4 model;
//
// void main()
// {
//     gl_Position = projection * view * model * vec4(aPos, 1.0);
// }

// In most of our samples we set a projection and view uniform matrix each render iteration for each shader we're using.
// This is a perfect example of where uniform buffer objects become useful since now we only have to store these matrices once.

// Here we declared a uniform block called Matrices that stores two 4x4 matrices.
// Variables in a uniform block can be directly accessed without the block name as a prefix.
// Then we store these matrix values in a buffer somewhere in the OpenGL code and each shader that declared this uniform block has
// access to the matrices.

// What does "layout (std140)"" mean? This statement specifies the memory layout used to store the content of the uniform block.
// In other words, it sets the uniform block layout.

// Uniform block layout

// The content of a uniform block is stored in a buffer object, which is nothing more than a reserved piece of memory.
// Because this piece of memory holds no information on what kind of data it holds, we need to tell OpenGL what parts of the memory
// corresponds to which uniform variables in the shader.

// Imagine you have the following uniform block in a shader:

// layout (std140) uniform ExampleBlock
// {
//     float value;
//     vec3  vector;
//     mat4  matrix;
//     float values[3];
//     bool  boolean;
//     int   integer;
// };

// What we want to know is the size (in bytes) and the offset (from the start of the block) of each of these variables
// so we can place them in the buffer in the correct order. The size of each of the elements is clearly stated in OpenGL and
// directly corresponds to C++ data types; vectors and matrices being (large) arrays of floats. What OpenGL doesn't clearly state
// is the spacing between the variables. This allows the hardware to position variables as it sees fit.
// Some hardware is able to place a vec3 adjacent to a float for example.
// Other hardware pads the vec3 to an array of 4 floats before appending the float.

// By default GLSL uses a uniform memory layout called a shared layout - shared because once the offsets are defined by the hardware,
// they are consistently shared between multiple programs. With a shared layout GLSL is allowed to reposition the uniform variables
// for optimization as long as the variables' order remains intact. Because we don't know at what offset each uniform variable will be
// we don't know how to precisely fill our uniform buffer. We can query this information with functions like glGetUniformIndices,
// but that is out of the scope of this discussion.

// While a shared layout gives us some space-saving optimizations, we'd need to query each offset for each uniform variable,
// which translates into a lot of work. The general practice is however to not use the shared layout, but to use the std140 layout instead.
// The std140 layout explicitly states the memory layout for each variable type by calculating their respective offsets based on a set
// of rules. Since this is explicitly mentioned we can manually figure out the offsets for each variable.

// Each variable has a...
// Base alignment: The space a variable takes (including padding) within a uniform block. This value is calculated using the std140 layout rules.
// Aligned offset: The byte offset of a variable from the start of the block.
// Note that the aligned offset of a variable must be equal to a multiple of its base alignment.

// The exact layout rules can be found here: https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
// We'll list the most common rules below.
// Each variable type in GLSL such as int, float and bool is defined to be a four-byte quantity with each entity of 4 bytes
// being represented as N.

// -----------------------------------------------------------------------------------------------------------------------------------------------------------
// | Type                        | Layout rule
// -----------------------------------------------------------------------------------------------------------------------------------------------------------
// | Scalar e.g. int or bool     | Each scalar has a base alignment of N.
// | Vector                      | Either 2N or 4N. This means that a vec3 has a base alignment of 4N.
// | Array of scalars or vectors | Each element has a base alignment equal to that of a vec4.
// | Matrices                    | Stored as a large array of column vectors, where each of those vectors has a base alignment of vec4.
// | Struct                      | Equal to the computed size of its elements according to the previous rules, but padded to a multiple of the size of a vec4.
// -----------------------------------------------------------------------------------------------------------------------------------------------------------

// Take for example the uniform block we introduced earlier:

// layout (std140) uniform ExampleBlock
// {
//                      // base alignment                      // aligned offset
//     float value;     // 4                                   // 0 
//     vec3 vector;     // 16 (padded up to vec4)              // 16  (must be multiple of 16 so 4->16)
//     mat4 matrix;     // 16 (each column treated as a vec4)  // 32  (column 0)
//                      // 16                                  // 48  (column 1)
//                      // 16                                  // 64  (column 2)
//                      // 16                                  // 80  (column 3)
//     float values[3]; // 16 (each element treated as a vec4) // 96  (values[0])
//                      // 16                                  // 112 (values[1])
//                      // 16                                  // 128 (values[2])
//     bool boolean;    // 4                                   // 144
//     int integer;     // 4                                   // 148
// };

// With the calculated offset values, based on the rules of the std140 layout, we can fill the buffer with the variable data
// at each offset using functions like glBufferSubData. While not the most efficient, the std140 layout does guarantee us that
// the memory layout remains the same over each program that declared this uniform block.

// There are two other layouts to choose from that require us to query each offset before filling the buffers.
// We've already seen the shared layout, so the remaining one is the packed layout. When using the packed layout,
// there is no guarantee that the layout will remain the same between programs (not shared) because it allows the compiler to optimize
// uniform variables away from the uniform block, which might differ per shader.

// Using uniform buffers

// First we need to create a uniform buffer object which is done via glGenBuffers. Then we need to bind it
// to the GL_UNIFORM_BUFFER target. Finally, we need to allocate memory by calling glBufferData.

// unsigned int uboExampleBlock;
// glGenBuffers(1, &uboExampleBlock);
// glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
// glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
// glBindBuffer(GL_UNIFORM_BUFFER, 0);

// Now whenever we want to update or insert data into uboExampleBlock, we simply bind it to the GL_UNIFORM_BUFFER target and
// use glBufferSubData to update its memory. We only have to update this uniform buffer once, and all the shaders that use it immediately
// start using its updated data. But how does OpenGL know what uniform buffers correspond to which uniform blocks?

// In OpenGL there is something called binding points, which can be used to link uniform buffers and uniform blocks.
// When we create a uniform buffer, we can link it to a binding point. After doing this, we can link a uniform block in a shader
// to the same binding point to connect it to the previously linked uniform buffer.

// To link a uniform block to a specific binding point we must call glUniformBlockBinding. In the example below we are linking
// a uniform block called "Lights" to binding point 2:

// unsigned int lights_index = glGetUniformBlockIndex(shaderA.ID, // Shader program that contains the uniform block
//                                                    "Lights");  // Name of the uniform block
//
// glUniformBlockBinding(shaderA.ID,                              // Shader program that contains the uniform block
//                       lights_index,                            // Uniform block index (retrieved via glGetUniformBlockIndex)
//                       2);                                      // Binding point

// Note that we have to repeat this process for each shader.

// Also note that from OpenGL version 4.2 and onwards it is also possible to store the binding point of a uniform block
// explicitly in the shader by adding another layout specifier. This saves us the calls to glGetUniformBlockIndex and
// glUniformBlockBinding. The following code sets the binding point of the "Lights" uniform block explicitly:

// layout(std140, binding = 2) uniform Lights { ... };

// After linking the uniform block to the binding point, we also need to link the uniform buffer object to the same binding point.
// This can be accomplished with either glBindBufferBase or glBindBufferRange.

// glBindBufferBase(GL_UNIFORM_BUFFER, // Target
//                  2,                 // Binding point
//                  uboExampleBlock);  // Uniform buffer object
//
// // or
//
// glBindBufferRange(GL_UNIFORM_BUFFER, // Target
//                   2,                 // Binding point
//                   uboExampleBlock,   // Uniform buffer object
//                   0,                 // Offset
//                   152);              // Size

// The glBindBufferRange function allows us to only bind a specific region of a buffer to a binding point.
// The region of our choice starts at offset and it covers the given size.
// Using this function we could have multiple different uniform blocks linked to a single uniform buffer object.

// Now that everything is set up, we can start adding data to the uniform buffer. We could add all the data as a single byte array,
// or we could update specific parts of the buffer using glBufferSubData.
// To update the boolean inside uboExampleBlock we could do the following:

// glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
// int b = true; // bools in GLSL are represented as 4 bytes, so we store it in an integer
// glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &b);
// glBindBuffer(GL_UNIFORM_BUFFER, 0);

// Uniform buffer objects have several advantages over single uniforms:
// - Setting a lot of uniforms at once is faster than setting multiple uniforms one at a time.
// - If you want to change the same uniform over several shaders, it is much easier to do this through a uniform buffer.
// - You can use a lot more uniforms in shaders using uniform buffer objects.
//   OpenGL has a limit to how much uniform data it can handle, which can be queried with GL_MAX_VERTEX_UNIFORM_COMPONENTS.
//   When using uniform buffer objects, this limit is much higher. So whenever you reach the maximum number of uniforms,
//   you can use uniform buffer objects.

// Geometry Shader

// Between the vertex and the fragment shader there is an optional shader stage called the geometry shader.
// A geometry shader takes as input a set of vertices that form a single primitive, and it is able to transform them to generate
// new primitives, possibly genearting new vertices in the process.

// Below is an example of a geometry shader that takes a point primitive as its input and creates a horizontal line primitive
// with the input point at its center.

// #version 330 core
//
// layout (points) in;
// layout (line_strip, max_vertices = 2) out;
// 
// void main()
// {
//     gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
//     EmitVertex();
// 
//     gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
//     EmitVertex();
// 
//     EndPrimitive();
// }

// At the start of every geometry shader we need to declare the type of primitive we are receiving from the vertex shader.
// We do this by declaring a layout specifier in front of the in keyword.
// This input layout qualifier can take any of the following primitive values:

// - points:              When drawing GL_POINTS (1).
// - lines:               When drawing GL_LINES or GL_LINE_STRIP (2).
// - lines_adjacency:     GL_LINES_ADJACENCY or GL_LINE_STRIP_ADJACENCY (4).
// - triangles:           GL_TRIANGLES, GL_TRIANGLE_STRIP or GL_TRIANGLE_FAN (3).
// - triangles_adjacency: GL_TRIANGLES_ADJACENCY or GL_TRIANGLE_STRIP_ADJACENCY (6).

// These are almost all the rendering primitives we are able to give to rendering calls like glDrawArrays.
// If we had chosen to draw vertices as GL_TRIANGLES we would need to set the input qualifier to triangles.
// The number within the parenthesis represents the minimum number of vertices that a single primitive can contain.

// We also need to specify the type of primitive that the geometry shader will output.
// We do this by declaring a layout specifier in front of the out keyword.
// Like the input layout qualifier, the output layout qualifier can also take several primitive values:

// - points
// - line_strip
// - triangle_strip

// With just these 3 values we can create almost any shape we want from the input primitives.
// To generate a single triangle, for example, we would need to specify triangle_strip as the output and then output 3 vertices.

// The geometry shader also expects us to set the maximum number of vertices it can output (if you exceed this number,
// OpenGL won't draw the extra vertices) which we can also do within the layout specifier of the out keyword.
// In this particular case we're going to output a line_strip with a maximum number of 2 vertices.

// In case you are wondering what a line strip is: a line strip binds together a set of points to form one continuous line between them,
// with a minimum of 2 points. Each extra point given to the rendering call results in a new line between the new point and the previous one.
// With the current shader we'll only output a single line since the maximum number of vertices is equal to 2.

// To generate meaningful results we need some way to retrieve the output of the previous shader stage.
// GLSL gives us a built-in variable called gl_in that looks like this internally:

// in gl_Vertex
// {
//     vec4  gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// } gl_in[];

// There are 3 things to note about gl_in:
// - It is declared as an interface block.
// - It contains gl_Position, which is the output variable that we always set in the vertex shader.
// - It is declared as an array, because most render primitives consist of more than 1 vertex and the geometry shader
//   receives all vertices of a primitive as its input.

// Using the vertex data from the vertex shader we can start generating new data, which is done via 2 geometry shader functions
// called EmitVertex and EndPrimitive.
// The geometry shader expects you to output at least one of the primitives you specified as output.
// In our case we want to at least generate one line strip primitive.

// void main()
// {
//     gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
//     EmitVertex();
//
//     gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
//     EmitVertex();
//
//     EndPrimitive();
// }

// Each time we call EmitVertex the vector currently set to gl_Position is added to the primitive.
// Whenever EndPrimitive is called, all emitted vertices are combined into the specified output render primitive.
// By repeatedly calling EndPrimitive after one or more EmitVertex calls, multiple primitives can be generated.
// This particular case emits two vertices that were translated by a small offset from the original vertex position,
// and then calls EndPrimitive, combining these two vertices into a single line strip of 2 vertices.

// Pass-through geometry shader:

// Let's render a scene where we just draw 4 points on the Z-plane in NDC.

// The coordinates of the points are:

// float points[] = {
//     -0.5f,  0.5f, // top-left
//      0.5f,  0.5f, // top-right
//      0.5f, -0.5f, // bottom-right
//     -0.5f, -0.5f  // bottom-left
// };

// The vertex shader looks like this:

// #version 330 core
// layout (location = 0) in vec2 aPos;
//
// void main()
// {
//     gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
// }

// And the fragment shader, which outputs the color green for all the points, looks like this:

// #version 330 core
//
// out vec4 FragColor;
//
// void main()
// {
//     FragColor = vec4(0.0, 1.0, 0.0, 1.0);
// }

// After generating a VBO and a VAO for the points' vertex data, we can draw them using glDrawArrays:

// shader.use();
// glBindVertexArray(VAO);
// glDrawArrays(GL_POINTS, 0, 4);

// The result is a dark scene with 4 green points.

// Now let's spice things up by adding a geometry shader to the scene.

// For learning purposes we'll start with what is called a pass-through geometry shader, which simply takes a point primitive
// as its input and passes it to the fragment shader unmodified:

// #version 330 core
// 
// layout (points) in;
// layout (points, max_vertices = 1) out;
// 
// void main()
// {
//     gl_Position = gl_in[0].gl_Position;
//     EmitVertex();
//     EndPrimitive();
// }

// A geometry shader needs to be compiled and linked to a program just like the vertex and fragment shader,
// but this time we'll create the shader using GL_GEOMETRY_SHADER as the shader type:

// geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
// glShaderSource(geometryShader, 1, &gShaderCode, NULL);
// glCompileShader(geometryShader);
// ...
// glAttachShader(program, geometryShader);
// glLinkProgram(program);

// The geometry shader compilation code is basically the same as the vertex and fragment shader compilation code.
// Be sure to check for compile or link errors!

// House geometry shader:

// Let's get creative by writing a geometry shader that draws a house at the location of each point we give it.
// We can achieve this by setting the output of the geometry shader to triangle_strip and then drawing a total
// of three triangles: two for a square and one for the roof.

// A triangle strip in OpenGL is a more efficient way to draw triangles using less vertices. After the first triangle is drawn,
// each subsequent vertex will generate another triangle next to the first triangle: every 3 adjacent vertices will form a triangle.
// If we have a total of 6 vertices that form a triangle strip we'd get the following triangles: (1,2,3), (2,3,4), (3,4,5) and (4,5,6).
// A triangle strip needs at least 3 vertices and will generate N-2 triangles; with 6 vertices we created 6-2 = 4 triangles.

// Using a triangle strip as the output of the geometry shader we can easily create the house shape we're after by generating
// 3 adjacent triangles in the correct order. The following image shows the order in which we need to draw the vertices to get the
// triangles we want:

//         5
//        / \
//       /   \
//      /     \
//     /       \
//   3 --------- 4
//     |\      |
//     | \     |
//     |  \    |
//     |   *   | layout(points) in
//     |    \  |
//     |     \ |
//     |      \|
//   1 -------- 2

// This translates to the following geometry shader:

// #version 330 core
// 
// layout (points) in;
// layout (triangle_strip, max_vertices = 5) out;
// 
// void build_house(vec4 position)
// {
//     gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1: bottom-left
//     EmitVertex();
//     gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2: bottom-right
//     EmitVertex();
//     gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3: top-left
//     EmitVertex();
//     gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4: top-right
//     EmitVertex();
//     gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5: top
//     EmitVertex();
//     EndPrimitive();
// }
// 
// void main()
// {
//     build_house(gl_in[0].gl_Position);
// }

// This geometry shader generates 5 vertices, with each vertex being the point's position plus an offset, to form one large
// triangle strip. The resulting primitive is then rasterized and the fragment shader runs on the entire triangle strip,
// resulting in a green house for each point we've drawn.

// House geometry shader with different colours:

// Let's give each house a unique colour.
// To do this we're going to add an extra vertex attribute with color information to the vertex shader, and we will receive that value
// in the geometry shader and forward it to the fragment shader.

// The updated vertex data is given below:

// float points[] = {
//     -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
//      0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
//      0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
//     -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
// };

// The updated vertex shader is given below. It now forwards the color attribute to the geometry shader using an interface block:

// #version 330 core
//
// layout (location = 0) in vec2 aPos;
// layout (location = 1) in vec3 aColor;
//
// out VS_OUT
// {
//     vec3 color;
// } vs_out;
//
// void main()
// {
//     gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
//     vs_out.color = aColor;
// }

// Then we also need to declare the same interface block (with a different interface name) in the geometry shader:

// in VS_OUT
// {
//     vec3 color;
// } gs_in[];

// Because the geometry shader acts on a set of vertices as its input, its input data from the vertex shader is always received
// in the form of arrays, even if we only receive a single vertex like we do right now.

// Note that we don't have to use interface blocks to transfer data to the geometry shader. We could have also written it as:

// in vec3 vColor[];

// If the vertex shader had forwarded the color vector as:

// out vec3 vColor;

// However, interface blocks are much easier to work with in shaders like the geometry shader. In practice, geometry shader inputs
// can get quite large and grouping them in one large interface block array makes things a lot easier to manage.

// Then we should also declare an output color vector for the next fragment shader stage:

// out vec3 fColor;

// Because the fragment shader expects only a single (interpolated) color it doesn't make sense to forward multiple colors.
// The fColor vector is thus not an array, but a single vector.
// ----------------------------------------------------------------
// When a vertex is emitted, it stores the current value of fColor.
// ----------------------------------------------------------------
// This happens because each vertex needs a colour so that it can be interpolated by the rasterizer and processed by the fragment shader.

// Since we want all the vertices that form a house to have the same colour, we can simply set fColor once and not change it:

// fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
//
// gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1: bottom-left
// EmitVertex();
// gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2: bottom-right
// EmitVertex();
// gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3: top-left
// EmitVertex();
// gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4: top-right
// EmitVertex();
// gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5: top
// EmitVertex();
// EndPrimitive();

// Just for fun we could also pretend it's winter and give the roofs of the houses a little snow by giving the last vertex a color of its own:

// fColor = gs_in[0].color;
// 
// gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:bottom-left
// EmitVertex();
// gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:bottom-right
// EmitVertex();
// gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:top-left
// EmitVertex();
// gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:top-right
// EmitVertex();
// gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:top
// fColor = vec3(1.0, 1.0, 1.0);
// EmitVertex();
// EndPrimitive();

// The full geometry shader end up looking like this:

// #version 330 core
// 
// layout (points) in;
// layout (triangle_strip, max_vertices = 5) out;
// 
// in VS_OUT
// {
//     vec3 color;
// } gs_in[];
// 
// out vec3 fColor;
// 
// void build_house(vec4 position)
// {
//     fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
//     gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); // 1:bottom-left
//     EmitVertex();
//     gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0); // 2:bottom-right
//     EmitVertex();
//     gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0); // 3:top-left
//     EmitVertex();
//     gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0); // 4:top-right
//     EmitVertex();
//     gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0); // 5:top
//     fColor = vec3(1.0, 1.0, 1.0);
//     EmitVertex();
//     EndPrimitive();
// }
// 
// void main()
// {
//     build_house(gl_in[0].gl_Position);
// }
