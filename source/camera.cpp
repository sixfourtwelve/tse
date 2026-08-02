#include "camera.hpp"

#include <OgreMath.h>
#include <OgreQuaternion.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_scancode.h>

#include <algorithm>

Camera::Camera(Ogre::SceneManager *sceneManager)
    : GameObject(sceneManager,
                 sceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)),
      mCamera(sceneManager->createCamera("Main Camera")),
      mPitchNode(mNode->createChildSceneNode(Ogre::SCENE_DYNAMIC)) {
  mNode->setPosition(0.0f, 5.0f, 20.0f);
  mCamera->setNearClipDistance(0.2f);
  mCamera->setFarClipDistance(1000.0f);
  mCamera->setAutoAspectRatio(true);
  attach(mCamera, mPitchNode);
}

void Camera::handleEvent(const SDL_Event &event) {
  if (event.type != SDL_MOUSEMOTION || SDL_GetRelativeMouseMode() != SDL_TRUE)
    return;

  constexpr float sensitivity = 0.0025f;
  mNode->yaw(Ogre::Radian(-event.motion.xrel * sensitivity),
             Ogre::SceneNode::TS_WORLD);

  mPitch = std::clamp(mPitch - event.motion.yrel * sensitivity,
                      -Ogre::Math::HALF_PI + 0.01f,
                      Ogre::Math::HALF_PI - 0.01f);
  mPitchNode->setOrientation(
      Ogre::Quaternion(Ogre::Radian(mPitch), Ogre::Vector3::UNIT_X));
}

void Camera::update(const float dt) {
  const Uint8 *keys = SDL_GetKeyboardState(nullptr);
  Ogre::Vector3 movement = Ogre::Vector3::ZERO;

  if (keys[SDL_SCANCODE_W])
    movement.z -= 1.0f;
  if (keys[SDL_SCANCODE_S])
    movement.z += 1.0f;
  if (keys[SDL_SCANCODE_A])
    movement.x -= 1.0f;
  if (keys[SDL_SCANCODE_D])
    movement.x += 1.0f;
  if (keys[SDL_SCANCODE_SPACE])
    movement.y += 1.0f;
  if (keys[SDL_SCANCODE_LCTRL])
    movement.y -= 1.0f;

  if (movement.squaredLength() == 0.0f)
    return;

  movement.normalise();
  const float speed = keys[SDL_SCANCODE_LSHIFT] ? 30.0f : 10.0f;
  mNode->translate(movement * speed * dt, Ogre::SceneNode::TS_LOCAL);
}

