#pragma once

#include "gameobject.hpp"

#include <OgreString.h>

class Model final : public GameObject {
public:
  Model(Ogre::SceneManager *sceneManager, Ogre::SceneNode *parent,
        const Ogre::String &meshName);
};
