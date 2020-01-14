#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <bitset>

#include "shader.h"

// TODO: Take advantage of inlining in this class.
class Window
{
public:

   Window(const std::string& title);
   ~Window();

   Window(const Window&) = delete;
   Window& operator=(const Window&) = delete;

   Window(Window&&) = delete;
   Window& operator=(Window&&) = delete;

   bool         initialize();

   bool         shouldClose() const;
   void         setShouldClose(bool shouldClose); // TODO: Could this be considered to be const?
   void         swapBuffers();                    // TODO: Could this be considered to be const?
   void         pollEvents();                     // TODO: Could this be considered to be const?

   // Window
   unsigned int getWidthInPix() const;
   unsigned int getHeightInPix() const;
   bool         isFullScreen() const;
   void         setFullScreen(bool fullScreen);

   // Keyboard
   bool         keyIsPressed(int key) const;
   bool         keyHasBeenProcessed(int key) const;
   void         setKeyAsProcessed(int key);

   // Cursor
   bool         mouseMoved() const;
   void         resetMouseMoved();
   void         resetFirstMove();
   float        getCursorXOffset() const;
   float        getCursorYOffset() const;
   void         enableCursor(bool enable);

   // Scroll wheel
   bool         scrollWheelMoved() const;
   void         resetScrollWheelMoved();
   float        getScrollYOffset() const;

   // Anti aliasing support
   bool configureAntiAliasingSupport();
   bool createMultisampleTexture(unsigned int numSamples);
   void prepareMultisampleTexture();
   void generateAndDisplayAntiAliasedImage();

private:

   void         setInputCallbacks();
   void         framebufferSizeCallback(GLFWwindow* window, int width, int height);
   void         keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
   void         cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
   void         scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

   // Window
   GLFWwindow*                    mWindow;
   unsigned int                   mWidthInPix;
   unsigned int                   mHeightInPix;
   std::string                    mTitle;
   bool                           mIsFullScreen;

   // Keyboard
   std::bitset<GLFW_KEY_LAST + 1> mKeys;
   std::bitset<GLFW_KEY_LAST + 1> mProcessedKeys;

   // Cursor
   bool                           mMouseMoved;
   bool                           mFirstCursorPosCallback;
   double                         mLastCursorXPos;
   double                         mLastCursorYPos;
   float                          mCursorXOffset;
   float                          mCursorYOffset;

   // Scroll wheel
   bool                           mScrollWheelMoved;
   float                          mScrollYOffset;

   // Anti aliasing support
   unsigned int                   mScreenVAO;
   unsigned int                   mRawMultisampleFBO;
   unsigned int                   mAntiAliasedFBO;
   unsigned int                   mAntiAliasedTexture;
   std::shared_ptr<Shader>        mScreenShader;
};

#endif
