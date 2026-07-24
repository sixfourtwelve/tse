#pragma once

#include "gameobject.hpp"
#include <OgreLight.h>
#include <OgreSceneManager.h>
#include <string>

class Light final : public GameObject {
public:
  Light(Ogre::SceneManager *scnMgr, Ogre::SceneNode *parentNode,
        const std::string &name,
        const Ogre::Light::LightTypes type = Ogre::Light::LT_POINT);
  void update(float dt) override;
};
