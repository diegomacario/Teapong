#include <utility>

#include "camera.h"

Camera::Camera(glm::vec3 position,
               glm::vec3 worldUp,
               float     yawInDeg,
               float     pitchInDeg,
               float     fieldOfViewYInDeg,
               float     aspectRatio,
               float     near,
               float     far,
               float     movementSpeed,
               float     mouseSensitivity)
   : mPosition(position)
   , mFront()
   , mUp()
   , mRight()
   , mWorldUp(worldUp)
   , mYawInDeg(yawInDeg)
   , mPitchInDeg(pitchInDeg)
   , mFieldOfViewYInDeg(fieldOfViewYInDeg)
   , mAspectRatio(aspectRatio)
   , mNear(near)
   , mFar(far)
   , mMovementSpeed(movementSpeed)
   , mMouseSensitivity(mouseSensitivity)
   , mIsFree(false)
{
   updateCoordinateFrame();
}

Camera::Camera(Camera&& rhs) noexcept
   : mPosition(std::exchange(rhs.mPosition, glm::vec3(0.0f)))
   , mFront(std::exchange(rhs.mFront, glm::vec3(0.0f)))
   , mUp(std::exchange(rhs.mUp, glm::vec3(0.0f)))
   , mRight(std::exchange(rhs.mRight, glm::vec3(0.0f)))
   , mWorldUp(std::exchange(rhs.mWorldUp, glm::vec3(0.0f)))
   , mYawInDeg(std::exchange(rhs.mYawInDeg, 0.0f))
   , mPitchInDeg(std::exchange(rhs.mPitchInDeg, 0.0f))
   , mFieldOfViewYInDeg(std::exchange(rhs.mFieldOfViewYInDeg, 0.0f))
   , mAspectRatio(std::exchange(rhs.mAspectRatio, 0.0f))
   , mNear(std::exchange(rhs.mNear, 0.0f))
   , mFar(std::exchange(rhs.mFar, 0.0f))
   , mMovementSpeed(std::exchange(rhs.mMovementSpeed, 0.0f))
   , mMouseSensitivity(std::exchange(rhs.mMouseSensitivity, 0.0f))
   , mIsFree(std::exchange(rhs.mIsFree, false))
{

}

Camera& Camera::operator=(Camera&& rhs) noexcept
{
   mPosition          = std::exchange(rhs.mPosition, glm::vec3(0.0f));
   mFront             = std::exchange(rhs.mFront, glm::vec3(0.0f));
   mUp                = std::exchange(rhs.mUp, glm::vec3(0.0f));
   mRight             = std::exchange(rhs.mRight, glm::vec3(0.0f));
   mWorldUp           = std::exchange(rhs.mWorldUp, glm::vec3(0.0f));
   mYawInDeg          = std::exchange(rhs.mYawInDeg, 0.0f);
   mPitchInDeg        = std::exchange(rhs.mPitchInDeg, 0.0f);
   mFieldOfViewYInDeg = std::exchange(rhs.mFieldOfViewYInDeg, 0.0f);
   mAspectRatio       = std::exchange(rhs.mAspectRatio, 0.0f);
   mNear              = std::exchange(rhs.mNear, 0.0f);
   mFar               = std::exchange(rhs.mFar, 0.0f);
   mMovementSpeed     = std::exchange(rhs.mMovementSpeed, 0.0f);
   mMouseSensitivity  = std::exchange(rhs.mMouseSensitivity, 0.0f);
   mIsFree            = std::exchange(rhs.mIsFree, false);
   return *this;
}

glm::vec3 Camera::getPosition()
{
   return mPosition;
}

glm::mat4 Camera::getViewMatrix()
{
   // TODO: Could we optimize things by storing the lookAt matrix and only recalculating it if the camera's settings changed?
   return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

glm::mat4 Camera::getPerspectiveProjectionMatrix()
{
   return glm::perspective(glm::radians(mFieldOfViewYInDeg),
                                        mAspectRatio,
                                        mNear,
                                        mFar);
}

void Camera::reposition(const glm::vec3& position,
                        const glm::vec3& worldUp,
                        float            yawInDeg,
                        float            pitchInDeg,
                        float            fieldOfViewYInDeg)
{
   mPosition          = position;
   mWorldUp           = worldUp;
   mYawInDeg          = yawInDeg;
   mPitchInDeg        = pitchInDeg;
   mFieldOfViewYInDeg = fieldOfViewYInDeg;

   updateCoordinateFrame();
}

void Camera::processKeyboardInput(MovementDirection direction, float deltaTime)
{
   float velocity = mMovementSpeed * deltaTime;

   switch (direction)
   {
   case MovementDirection::Forward:
      mPosition += mFront * velocity;
      break;
   case MovementDirection::Backward:
      mPosition -= mFront * velocity;
      break;
   case MovementDirection::Left:
      mPosition -= mRight * velocity;
      break;
   case MovementDirection::Right:
      mPosition += mRight * velocity;
      break;
   }
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
   // TODO: Should the offsets also be scaled by deltaTime?
   xOffset *= mMouseSensitivity;
   yOffset *= mMouseSensitivity;

   mYawInDeg   += xOffset;
   mPitchInDeg += yOffset;

   // Make sure that when the pitch is out of bounds, the screen doesn't get flipped
   if (mPitchInDeg > 89.0f)
   {
      mPitchInDeg = 89.0f;
   }
   else if (mPitchInDeg < -89.0f)
   {
      mPitchInDeg = -89.0f;
   }

   // Update the front, right and up vectors using the updated Euler angles
   updateCoordinateFrame();
}

void Camera::processScrollWheelMovement(float yOffset)
{
   // The larger the FOV, the smaller things appear on the screen
   // The smaller the FOV, the larger things appear on the screen
   if (mFieldOfViewYInDeg >= 1.0f && mFieldOfViewYInDeg <= 45.0f)
   {
      mFieldOfViewYInDeg -= yOffset;
   }
   else if (mFieldOfViewYInDeg < 1.0f)
   {
      mFieldOfViewYInDeg = 1.0f;
   }
   else if (mFieldOfViewYInDeg > 45.0f)
   {
      mFieldOfViewYInDeg = 45.0f;
   }
}

bool Camera::isFree() const
{
   return mIsFree;
}

void Camera::setFree(bool free)
{
   mIsFree = free;
}

void Camera::updateCoordinateFrame()
{
   // Calculate the new front vector
   glm::vec3 newFront;
   newFront.x = sin(glm::radians(mYawInDeg)) * cos(glm::radians(mPitchInDeg));
   newFront.y = sin(glm::radians(mPitchInDeg));
   newFront.z = -1.0f * cos(glm::radians(mYawInDeg)) * cos(glm::radians(mPitchInDeg));
   mFront = glm::normalize(newFront);

   // Calculate the new right and up vectors
   mRight = glm::normalize(glm::cross(mFront, mWorldUp));
   mUp    = glm::normalize(glm::cross(mRight, mFront));
}
