#pragma once

#include "gameobject.hpp"

#include <OgreLight.h>

class Light final : public GameObject {
public:
  Light(Ogre::SceneManager *sceneManager, Ogre::SceneNode *parent);
  void update(float dt) override;

private:
  float mAngle = 0.0f;
};

