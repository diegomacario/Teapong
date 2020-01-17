#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include "game.h"

class PlayState : public State
{
public:

   PlayState(const std::shared_ptr<FiniteStateMachine>&     finiteStateMachine,
             const std::shared_ptr<Window>&                 window,
             const std::shared_ptr<irrklang::ISoundEngine>& soundEngine,
             const std::shared_ptr<Camera>&                 camera,
             const std::shared_ptr<Shader>&                 gameObject3DShader,
             const std::shared_ptr<GameObject3D>&           table,
             const std::shared_ptr<Paddle>&                 leftPaddle,
             const std::shared_ptr<Paddle>&                 rightPaddle,
             const std::shared_ptr<Ball>&                   ball);
   ~PlayState() = default;

   PlayState(const PlayState&) = delete;
   PlayState& operator=(const PlayState&) = delete;

   PlayState(PlayState&&) = delete;
   PlayState& operator=(PlayState&&) = delete;

   void enter() override;
   void processInputAndUpdate(float deltaTime) override;
   void render() override;
   void exit() override;

private:

   void processInput(float deltaTime);
   void update(float deltaTime);

   void calculateInitialDirectionOfBall();

   bool ballIsOutsideOfHorizontalRange();

   void updateScore();

   void resetScene();

   void resetCamera();

   void playSoundOfCollision();

   std::shared_ptr<FiniteStateMachine>     mFSM;

   std::shared_ptr<Window>                 mWindow;

   std::shared_ptr<irrklang::ISoundEngine> mSoundEngine;

   std::shared_ptr<Camera>                 mCamera;

   std::shared_ptr<Shader>                 mGameObject3DShader;

   std::shared_ptr<GameObject3D>           mTable;
   std::shared_ptr<Paddle>                 mLeftPaddle;
   std::shared_ptr<Paddle>                 mRightPaddle;
   std::shared_ptr<Ball>                   mBall;

   bool                                    mBallIsInPlay;
   bool                                    mBallIsFalling;

   unsigned int                            mPointsScoredByLeftPaddle;
   unsigned int                            mPointsScoredByRightPaddle;
};

#endif
