#ifndef PAUSE_STATE_H
#define PAUSE_STATE_H

#include "game.h"

class PauseState : public State
{
public:

   PauseState(const std::shared_ptr<FiniteStateMachine>& finiteStateMachine,
              const std::shared_ptr<Window>&             window,
              const std::shared_ptr<Camera>&             camera,
              const std::shared_ptr<Shader>&             gameObject3DShader,
              const std::shared_ptr<GameObject3D>&       table,
              const std::shared_ptr<Paddle>&             leftPaddle,
              const std::shared_ptr<Paddle>&             rightPaddle,
              const std::shared_ptr<Ball>&               ball);
   ~PauseState() = default;

   PauseState(const PauseState&) = delete;
   PauseState& operator=(const PauseState&) = delete;

   PauseState(PauseState&&) = delete;
   PauseState& operator=(PauseState&&) = delete;

   void enter() override;
   void execute(float deltaTime) override;
   void exit() override;

private:

   void processInput(float deltaTime);
   void render();

   void resetCamera();

   std::shared_ptr<FiniteStateMachine> mFSM;

   std::shared_ptr<Window>             mWindow;

   std::shared_ptr<Camera>             mCamera;

   std::shared_ptr<Shader>             mGameObject3DShader;

   std::shared_ptr<GameObject3D>       mTable;
   std::shared_ptr<Paddle>             mLeftPaddle;
   std::shared_ptr<Paddle>             mRightPaddle;
   std::shared_ptr<Ball>               mBall;

   bool                                mCameraIsFree;
};

#endif
