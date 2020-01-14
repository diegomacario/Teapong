#include <iostream>

#include "resource_manager.h"
#include "shader_loader.h"
#include "window.h"

Window::Window(const std::string& title)
   : mWindow(nullptr)
   , mWidthInPix(0)
   , mHeightInPix(0)
   , mTitle(title)
   , mIsFullScreen(true)
   , mKeys()
   , mProcessedKeys()
   , mMouseMoved(false)
   , mFirstCursorPosCallback(true)
   , mLastCursorXPos(0.0)
   , mLastCursorYPos(0.0)
   , mCursorXOffset(0.0)
   , mCursorYOffset(0.0)
   , mScrollWheelMoved(false)
   , mScrollYOffset(0.0)
   , mScreenVAO(0)
   , mRawMultisampleFBO(0)
   , mAntiAliasedFBO(0)
   , mAntiAliasedTexture(0)
   , mScreenShader()
{

}

Window::~Window()
{
   glDeleteVertexArrays(1, &mScreenVAO);
   glDeleteFramebuffers(1, &mRawMultisampleFBO);
   glDeleteFramebuffers(1, &mAntiAliasedFBO);
   glDeleteTextures(1, &mAntiAliasedTexture);

   if (mWindow)
   {
      glfwTerminate();
      mWindow = nullptr;
   }
}

bool Window::initialize()
{
   if (!glfwInit())
   {
      std::cout << "Error - Window::initialize - Failed to initialize GLFW" << "\n";
      return false;
   }

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   mWidthInPix = mode->width;
   mHeightInPix = mode->height;
   mWindow = glfwCreateWindow(mWidthInPix, mHeightInPix, mTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
   if (!mWindow)
   {
      std::cout << "Error - Window::initialize - Failed to create the GLFW window" << "\n";
      glfwTerminate();
      mWindow = nullptr;
      return false;
   }

   // TODO TODO: Remove this
   setFullScreen(false);

   glfwMakeContextCurrent(mWindow);

   setInputCallbacks();

   // TODO TODO: Change this to false
   enableCursor(true);

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cout << "Error - Window::initialize - Failed to load pointers to OpenGL functions using GLAD" << "\n";
      glfwTerminate();
      mWindow = nullptr;
      return false;
   }

   glViewport(0, 0, mWidthInPix, mHeightInPix);
   glEnable(GL_CULL_FACE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   if (!configureAntiAliasingSupport())
   {
      std::cout << "Error - Window::initialize - Failed to configure anti aliasing support" << "\n";
      glfwTerminate();
      mWindow = nullptr;
      return false;
   }

   return true;
}

bool Window::shouldClose() const
{
   int windowShouldClose = glfwWindowShouldClose(mWindow);

   if (windowShouldClose == 0)
   {
      return false;
   }
   else
   {
      return true;
   }
}

void Window::setShouldClose(bool shouldClose)
{
   glfwSetWindowShouldClose(mWindow, shouldClose);
}

void Window::swapBuffers()
{
   glfwSwapBuffers(mWindow);
}

void Window::pollEvents()
{
   glfwPollEvents();
}

unsigned int Window::getWidthInPix() const
{
   return mWidthInPix;
}

unsigned int Window::getHeightInPix() const
{
   return mHeightInPix;
}

bool Window::isFullScreen() const
{
   return mIsFullScreen;
}

void Window::setFullScreen(bool fullScreen)
{
   if (fullScreen)
   {
      const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      mWidthInPix = mode->width;
      mHeightInPix = mode->height;
      glfwSetWindowMonitor(mWindow, glfwGetPrimaryMonitor(), 0, 0, mWidthInPix, mHeightInPix, GLFW_DONT_CARE);
   }
   else
   {
      mWidthInPix = 1280;
      mHeightInPix = 720;
      glfwSetWindowMonitor(mWindow, NULL, 20, 50, mWidthInPix, mHeightInPix, GLFW_DONT_CARE);
   }

   mIsFullScreen = fullScreen;
}

bool Window::keyIsPressed(int key) const
{
   return mKeys.test(key);
}

bool Window::keyHasBeenProcessed(int key) const
{
   return mProcessedKeys.test(key);
}

void Window::setKeyAsProcessed(int key)
{
   mProcessedKeys.set(key);
}

bool Window::mouseMoved() const
{
   return mMouseMoved;
}

void Window::resetMouseMoved()
{
   mMouseMoved = false;
}

void Window::resetFirstMove()
{
   mFirstCursorPosCallback = true;
}

float Window::getCursorXOffset() const
{
   return mCursorXOffset;
}

float Window::getCursorYOffset() const
{
   return mCursorYOffset;
}

void Window::enableCursor(bool enable)
{
      glfwSetInputMode(mWindow, GLFW_CURSOR, enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool Window::scrollWheelMoved() const
{
   return mScrollWheelMoved;
}

void Window::resetScrollWheelMoved()
{
   mScrollWheelMoved = false;
}

float Window::getScrollYOffset() const
{
   return mScrollYOffset;
}

void Window::setInputCallbacks()
{
   glfwSetWindowUserPointer(mWindow, this);

   auto framebufferSizeCallback = [](GLFWwindow* window, int width, int height)
   {
      static_cast<Window*>(glfwGetWindowUserPointer(window))->framebufferSizeCallback(window, width, height);
   };

   auto keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods)
   {
      static_cast<Window*>(glfwGetWindowUserPointer(window))->keyCallback(window, key, scancode, action, mods);
   };

   auto cursorPosCallback = [](GLFWwindow* window, double xPos, double yPos)
   {
      static_cast<Window*>(glfwGetWindowUserPointer(window))->cursorPosCallback(window, xPos, yPos);
   };

   auto scrollCallback = [](GLFWwindow* window, double xOffset, double yOffset)
   {
      static_cast<Window*>(glfwGetWindowUserPointer(window))->scrollCallback(window, xOffset, yOffset);
   };

   glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
   glfwSetKeyCallback(mWindow, keyCallback);
   glfwSetCursorPosCallback(mWindow, cursorPosCallback);
   glfwSetScrollCallback(mWindow, scrollCallback);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
   mWidthInPix = width;
   mHeightInPix = height;
   glViewport(0, 0, width, height);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key != -1)
   {
      if (action == GLFW_PRESS)
      {
         mKeys.set(key);
      }
      else if (action == GLFW_RELEASE)
      {
         mKeys.reset(key);
      }

      mProcessedKeys.reset(key);
   }
}

void Window::cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
   if (mFirstCursorPosCallback)
   {
      mLastCursorXPos = xPos;
      mLastCursorYPos = yPos;
      mFirstCursorPosCallback = false;
   }

   // TODO: Ideally this function would tell the camera to update its position based on the offsets.
   // I'm going to make the camera ask the window if it should update its position. Is there a better way to do this?
   mCursorXOffset = static_cast<float>(xPos - mLastCursorXPos);
   mCursorYOffset = static_cast<float>(mLastCursorYPos - yPos); // Reversed since the Y-coordinates of the mouse go from the bottom to the top

   mLastCursorXPos = xPos;
   mLastCursorYPos = yPos;

   mMouseMoved = true;
}

void Window::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
   // TODO: Ideally this function would tell the camera to update its FOVY based on the Y offset.
   // I'm going to make the camera ask the window if it should update its FOVY. Is there a better way to do this?
   mScrollYOffset = static_cast<float>(yOffset);

   mScrollWheelMoved = true;
}

bool Window::configureAntiAliasingSupport()
{
   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates
   float screenVertAttributes[] = {
       // Positions   // Tex coords
       -1.0f,  1.0f,  0.0f, 1.0f,
       -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

       -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
   };

   unsigned int screenVBO;
   glGenVertexArrays(1, &mScreenVAO);
   glGenBuffers(1, &screenVBO);

   glBindVertexArray(mScreenVAO);

   // Load the quad's data into the buffers

   // Positions, normals and texture coordinates
   glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertAttributes), &screenVertAttributes, GL_STATIC_DRAW);

   // Set the vertex attribute pointers

   // Positions
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
   // Texture coords
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

   glBindVertexArray(0);
   glDeleteBuffers(1, &screenVBO);

   if (!createMultisampleTexture(4))
   {
      return false;
   }

   // Configure a framebuffer object to store anti aliased renders

   glGenFramebuffers(1, &mAntiAliasedFBO);
   glBindFramebuffer(GL_FRAMEBUFFER, mAntiAliasedFBO);

   // Create a texture and use it as a color attachment
   glGenTextures(1, &mAntiAliasedTexture);
   glBindTexture(GL_TEXTURE_2D, mAntiAliasedTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidthInPix, mHeightInPix, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mAntiAliasedTexture, 0);

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
   {
      std::cout << "Error - Window::configureAntiAliasingSupport - Anti aliased framebuffer is not complete" << "\n";
      return false;
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   // Initialize the screen shader
   mScreenShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/screen.vs",
                                                                                 "resources/shaders/screen.fs");

   mScreenShader->use();
   mScreenShader->setInt("screenTexture", 0);

   return true;
}

bool Window::createMultisampleTexture(unsigned int numSamples)
{
   // Configure a framebuffer object to store raw multisample renders

   glGenFramebuffers(1, &mRawMultisampleFBO);

   glBindFramebuffer(GL_FRAMEBUFFER, mRawMultisampleFBO);

   // Create a multisample texture and use it as a color attachment
   unsigned int rawMultisampleTexture;
   glGenTextures(1, &rawMultisampleTexture);

   glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, rawMultisampleTexture);
   glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_RGB, mWidthInPix, mHeightInPix, GL_TRUE);
   glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, rawMultisampleTexture, 0);

   // Create a multisample renderbuffer object and use it as a depth attachment
   unsigned int multisampleRBO;
   glGenRenderbuffers(1, &multisampleRBO);

   glBindRenderbuffer(GL_RENDERBUFFER, multisampleRBO);
   glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, GL_DEPTH_COMPONENT32F, mWidthInPix, mHeightInPix);
   glBindRenderbuffer(GL_RENDERBUFFER, 0);

   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multisampleRBO);

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
   {
      std::cout << "Error - Window::configureAntiAliasingSupport - Raw multisample framebuffer is not complete" << "\n";
      return false;
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glDeleteTextures(1, &rawMultisampleTexture);
   glDeleteRenderbuffers(1, &multisampleRBO);
}

void Window::prepareMultisampleTexture()
{
   glBindFramebuffer(GL_FRAMEBUFFER, mRawMultisampleFBO);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::generateAndDisplayAntiAliasedImage()
{
   glBindFramebuffer(GL_READ_FRAMEBUFFER, mRawMultisampleFBO);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mAntiAliasedFBO);
   glBlitFramebuffer(0, 0, mWidthInPix, mHeightInPix, 0, 0, mWidthInPix, mHeightInPix, GL_COLOR_BUFFER_BIT, GL_NEAREST);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glClear(GL_COLOR_BUFFER_BIT);
   glDisable(GL_DEPTH_TEST);

   // Draw screen quad
   mScreenShader->use();
   glBindVertexArray(mScreenVAO);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mAntiAliasedTexture); // Use the now resolved color attachment as the quad's texture
   glDrawArrays(GL_TRIANGLES, 0, 6);
}
