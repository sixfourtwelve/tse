#pragma once

#include "gameobject.hpp"

#include <OgreCamera.h>
#include <SDL_events.h>

class Camera final : public GameObject {
public:
  explicit Camera(Ogre::SceneManager *sceneManager);

  Ogre::Camera *getCamera() const { return mCamera; }

  void handleEvent(const SDL_Event &event);
  void update(float dt) override;

private:
  Ogre::Camera *mCamera;
  Ogre::SceneNode *mPitchNode;
  float mPitch = 0.0f;
};

