#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <bitset>

// TODO: Take advantage of inlining in this class.
class Window
{
public:

   Window(unsigned int widthInPix, unsigned int heightInPix, const std::string& title);
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

   unsigned int getWidthInPix() const;
   unsigned int getHeightInPix() const;

   // Keyboard
   bool         keyIsPressed(int key) const;
   bool         keyHasBeenProcessed(int key) const;
   void         setKeyAsProcessed(int key);

   // Cursor
   bool         mouseMoved() const;
   void         resetMouseMoved();
   float        getCursorXOffset() const;
   float        getCursorYOffset() const;

   // Scroll wheel
   bool         scrollWheelMoved() const;
   void         resetScrollWheelMoved();
   float        getScrollYOffset() const;

private:

   void         setInputCallbacks();
   void         framebufferSizeCallback(GLFWwindow* window, int width, int height);
   void         keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
   void         cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
   void         scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

   GLFWwindow*                    mWindow;

   unsigned int                   mWidthInPix;
   unsigned int                   mHeightInPix;
   std::string                    mTitle;

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
};

#endif
