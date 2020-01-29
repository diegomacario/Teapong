#include <stb_image_write.h>

#include "play_state.h"
#include "win_state.h"

WinState::WinState(const std::shared_ptr<FiniteStateMachine>& finiteStateMachine,
                   const std::shared_ptr<Window>&             window,
                   const std::shared_ptr<Camera>&             camera,
                   const std::shared_ptr<Shader>&             gameObject3DExplosiveShader,
                   const std::shared_ptr<Ball>&               ball,
                   const std::shared_ptr<GameObject3D>&       leftPaddleWins,
                   const std::shared_ptr<GameObject3D>&       rightPaddleWins)
   : mFSM(finiteStateMachine)
   , mWindow(window)
   , mCamera(camera)
   , mGameObject3DExplosiveShader(gameObject3DExplosiveShader)
   , mBall(ball)
   , mLeftPaddleWins(leftPaddleWins)
   , mRightPaddleWins(rightPaddleWins)
   , mCameraPosition(0.0f, -30.0f, 10.0f)
   , mCameraTarget(0.0f, 0.0f, 5.0f)
   , mCameraUp(0.0f, 0.0f, 1.0f)
   , mCameraRight(0.0f)
   , mIdleOrbitalAngularVelocity(-50.0f)
   , mTimeWhenExplosionShouldBegin(0.0)
   , mExplode(false)
   , mSpeedOfExplodingFragments(2.0f)
   , mDistanceTravelledByExplodingFragments(0.0f)
   , mWinner()
   , mTimeWhenWinnerIsFirstDisplayed(0.0)
   , mDisplayWinner(false)
{

}

void WinState::enter()
{
   // In the win state, the cursor is disabled when fullscreen and enabled when windowed
   mWindow->enableCursor(!mWindow->isFullScreen());

   mBall->reset();
   float currRadius = mBall->getRadius();
   mBall->scale(7.5f / currRadius);
   mBall->setRadius(7.5f);

   mCameraPosition = glm::vec3(0.0f, -30.0f, 10.0f);
   mCameraTarget   = glm::vec3(0.0f, 0.0f, 5.0f);
   mCameraUp       = glm::vec3(0.0f, 0.0f, 1.0f);
   mCameraRight    = glm::vec3(0.0f);

   mTimeWhenExplosionShouldBegin          = glfwGetTime() + 3.5;
   mExplode                               = false;
   mSpeedOfExplodingFragments             = 2.0f;
   mDistanceTravelledByExplodingFragments = 0.0f;

   const PlayState& playState = dynamic_cast<PlayState&>(*mFSM->getPreviousState());
   if (playState.getPointsScoredByLeftPaddle() == 3)
   {
      mWinner = Winner::leftPaddleWon;
   }
   else
   {
      mWinner = Winner::rightPaddleWon;
   }

   mTimeWhenWinnerIsFirstDisplayed = 0.0;
   mDisplayWinner = false;

   mExplode = true;
}

static bool record = true;

void WinState::processInput(float deltaTime)
{
   // Close the game
   if (mWindow->keyIsPressed(GLFW_KEY_ESCAPE)) { mWindow->setShouldClose(true); }

   // Make the game full screen or windowed
   if (mWindow->keyIsPressed(GLFW_KEY_F) && !mWindow->keyHasBeenProcessed(GLFW_KEY_F))
   {
      mWindow->setKeyAsProcessed(GLFW_KEY_F);
      mWindow->setFullScreen(!mWindow->isFullScreen());
      mWindow->enableCursor(!mWindow->isFullScreen());
   }

   // Change the number of samples used for anti aliasing
   if (mWindow->keyIsPressed(GLFW_KEY_1) && !mWindow->keyHasBeenProcessed(GLFW_KEY_1))
   {
      mWindow->setKeyAsProcessed(GLFW_KEY_1);
      mWindow->setNumberOfSamples(1);
   }
   else if (mWindow->keyIsPressed(GLFW_KEY_2) && !mWindow->keyHasBeenProcessed(GLFW_KEY_2))
   {
      mWindow->setKeyAsProcessed(GLFW_KEY_2);
      mWindow->setNumberOfSamples(2);
   }
   else if (mWindow->keyIsPressed(GLFW_KEY_4) && !mWindow->keyHasBeenProcessed(GLFW_KEY_4))
   {
      mWindow->setKeyAsProcessed(GLFW_KEY_4);
      mWindow->setNumberOfSamples(4);
   }
   else if (mWindow->keyIsPressed(GLFW_KEY_8) && !mWindow->keyHasBeenProcessed(GLFW_KEY_8))
   {
      mWindow->setKeyAsProcessed(GLFW_KEY_8);
      mWindow->setNumberOfSamples(8);
   }
}

static bool reverse = false;
static bool done = false;

void WinState::update(float deltaTime)
{
   mBall->setPosition(glm::vec3(0.0f, 0.0f, mBall->getScalingFactor() * 1.96875));

   if (!mDisplayWinner && mDistanceTravelledByExplodingFragments > 10.0f) // Hey
   {
      // Reset the position of the camera
      //mCameraPosition = glm::vec3(0.0f, -30.0f, 0.0f);
      //mCameraTarget   = glm::vec3(0.0f, 0.0f, 0.0f);
      //mCameraUp       = glm::vec3(0.0f, 0.0f, 1.0f);
      //mCameraRight    = glm::vec3(0.0f);

      //mTimeWhenWinnerIsFirstDisplayed = glfwGetTime();

      //mDisplayWinner = true;
      reverse = true;
      mIdleOrbitalAngularVelocity = -mIdleOrbitalAngularVelocity;
   }
   else if (!mDisplayWinner && mDistanceTravelledByExplodingFragments <= 0.0f && reverse) // Hey
   {
      // Reset the position of the camera
      //mCameraPosition = glm::vec3(0.0f, -30.0f, 0.0f);
      //mCameraTarget   = glm::vec3(0.0f, 0.0f, 0.0f);
      //mCameraUp       = glm::vec3(0.0f, 0.0f, 1.0f);
      //mCameraRight    = glm::vec3(0.0f);

      //mTimeWhenWinnerIsFirstDisplayed = glfwGetTime();

      //mDisplayWinner = true;
      done = true;
      mFSM->changeState("menu");
   }

   if (mDisplayWinner && (glfwGetTime() > mTimeWhenWinnerIsFirstDisplayed + 1))
   {
      //mFSM->changeState("menu");
   }

   if (!mExplode && (glfwGetTime() > mTimeWhenExplosionShouldBegin))
   {
      mExplode = true;
   }

   mBall->setPosition(glm::vec3(0.0f, 0.0f, mBall->getScalingFactor() * 1.96875));

   if (mExplode && !reverse)
   {
      mSpeedOfExplodingFragments += 0.025f;
      mDistanceTravelledByExplodingFragments += mSpeedOfExplodingFragments * deltaTime;
   }
   else if (mExplode && reverse)
   {
      mSpeedOfExplodingFragments -= 0.025f;
      mDistanceTravelledByExplodingFragments -= mSpeedOfExplodingFragments * deltaTime;
   }
   else
   {
      //mBall->moveInFreeFall(deltaTime);
      mBall->setPosition(glm::vec3(0.0f, 0.0f, mBall->getScalingFactor() * 1.96875));
   }

   if (!mDisplayWinner)
   {
      // Rotate the camera CW around the positive Z axis
      glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(mIdleOrbitalAngularVelocity * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
      mCameraPosition = glm::mat3(rotationMatrix) * mCameraPosition;
   }
}

static int frameNum = 0;
static std::string frameName = "C:\\repos\\Teapong\\VS2019_solution\\x64\\Release\\frames\\";
static GLubyte* data = new GLubyte[3 * 640 * 360];

void WinState::render()
{
   mWindow->clearAndBindMultisampleFramebuffer();

   // Enable depth testing for 3D objects
   glEnable(GL_DEPTH_TEST);

   mGameObject3DExplosiveShader->use();
   mGameObject3DExplosiveShader->setMat4("projectionView", mCamera->getPerspectiveProjectionMatrix() * glm::lookAt(mCameraPosition, mCameraTarget, mCameraUp));
   mGameObject3DExplosiveShader->setVec3("cameraPos", mCameraPosition);
   if (mExplode && !mDisplayWinner)
   {
      mGameObject3DExplosiveShader->setFloat("distanceToMove", mDistanceTravelledByExplodingFragments);
   }
   else
   {
      mGameObject3DExplosiveShader->setFloat("distanceToMove", 0.0f);
   }

   if (mDisplayWinner)
   {
      if (mWinner == Winner::leftPaddleWon)
      {
         mLeftPaddleWins->render(*mGameObject3DExplosiveShader);
      }
      else
      {
         mRightPaddleWins->render(*mGameObject3DExplosiveShader);
      }
   }
   else
   {
      // Disable face culling so that we render the inside of the teapot
      glDisable(GL_CULL_FACE);
      mBall->render(*mGameObject3DExplosiveShader);
      glEnable(GL_CULL_FACE);
   }

   mWindow->generateAntiAliasedImage();

   if (record)
   {
      stbi_flip_vertically_on_write(true);
      int winWidth  = mWindow->getWidthOfFramebufferInPix();
      int winHeight = mWindow->getHeightOfFramebufferInPix();
      //GLubyte* data = static_cast<GLubyte*>(malloc(3 * winWidth * winHeight));
      memset(data, 0, 3 * winWidth * winHeight);
      //glReadBuffer( GL_BACK );
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glReadPixels(0, 0,
         winWidth, winHeight,
         GL_RGB, GL_UNSIGNED_BYTE, data);

      std::string imgName = frameName + std::to_string(frameNum) + ".png";
      stbi_write_png(imgName.c_str(), winWidth, winHeight, 3, data, winWidth * 3);
      frameNum++;
   }

   mWindow->swapBuffers();
   mWindow->pollEvents();
}

void WinState::exit()
{

}
