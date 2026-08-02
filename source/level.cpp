#include "level.hpp"

#include "cube.hpp"
#include "light.hpp"

#include <OgreColourValue.h>

Level::Level(Ogre::SceneManager *sceneManager)
    : mSceneManager(sceneManager),
      mLevelNode(sceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)
                     ->createChildSceneNode(Ogre::SCENE_DYNAMIC)) {
  sceneManager->setAmbientLight(
      Ogre::ColourValue(0.08f, 0.11f, 0.16f),
      Ogre::ColourValue(0.025f, 0.02f, 0.018f), Ogre::Vector3::UNIT_Y);

  spawn<Light>(Ogre::Vector3::ZERO);
  spawn<Cube>(Ogre::Vector3(0.0f, 0.0f, 0.0f));
  spawn<Cube>(Ogre::Vector3(4.0f, 0.0f, 0.0f),
              Ogre::ColourValue(0.2f, 0.6f, 1.0f));
  spawn<Cube>(Ogre::Vector3(-4.0f, 0.0f, 0.0f),
              Ogre::ColourValue(0.9f, 0.7f, 0.2f));
}

Level::~Level() {
  mGameObjects.clear();
  mSceneManager->destroySceneNode(mLevelNode);
}

void Level::update(const float dt) const {
  for (const auto &object : mGameObjects)
    object->update(dt);
}

