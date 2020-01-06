#include <iostream>

#include "window.h"

Window::Window(unsigned int widthInPix, unsigned int heightInPix, const std::string& title)
   : mWindow(nullptr)
   , mWidthInPix(widthInPix)
   , mHeightInPix(heightInPix)
   , mTitle(title)
   , mWindowIsFullScreen(true)
   , mCameraIsFree(false)
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
{

}

Window::~Window()
{
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
   glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // TODO: Can we keep it resizable? And how can we offer a fullscreen option?
   glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   mWindow = glfwCreateWindow(mWidthInPix, mHeightInPix, mTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
   if (!mWindow)
   {
      std::cout << "Error - Window::initialize - Failed to create the GLFW window" << "\n";
      glfwTerminate();
      mWindow = nullptr;
      return false;
   }

   glfwMakeContextCurrent(mWindow);

   setInputCallbacks();

   glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
   glEnable(GL_MULTISAMPLE);

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
   return mWindowIsFullScreen;
}

void Window::setFullScreen(bool fullScreen)
{
   if (fullScreen)
   {
      glfwSetWindowMonitor(mWindow, glfwGetPrimaryMonitor(), 0, 0, 1280, 720, GLFW_DONT_CARE);
   }
   else
   {
      glfwSetWindowMonitor(mWindow, NULL, 20, 50, mWidthInPix, mHeightInPix, GLFW_DONT_CARE);
   }

   mWindowIsFullScreen = fullScreen;
}

bool Window::isCameraFree() const
{
   return mCameraIsFree;
}

void Window::setCameraFree(bool free)
{
    mCameraIsFree = free;
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

float Window::getCursorXOffset() const
{
   return mCursorXOffset;
}

float Window::getCursorYOffset() const
{
   return mCursorYOffset;
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
