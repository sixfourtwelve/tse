#pragma once

#include "gameobject.hpp"

#include <OgreColourValue.h>
#include <OgreVector3.h>

#include <optional>

class Cube final : public GameObject {
public:
  Cube(Ogre::SceneManager *sceneManager, Ogre::SceneNode *parent,
       std::optional<Ogre::ColourValue> colour = std::nullopt,
       const Ogre::Vector3 &size = Ogre::Vector3(2.0f));

private:
  static Ogre::String ensureMesh(Ogre::SceneManager *sceneManager,
                                 const Ogre::String &datablockName);
};
