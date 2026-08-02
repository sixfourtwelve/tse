#include "model.hpp"

#include <OgreItem.h>

Model::Model(Ogre::SceneManager *sceneManager, Ogre::SceneNode *parent,
             const Ogre::String &meshName)
    : GameObject(sceneManager, parent) {
  Ogre::Item *item =
      sceneManager->createItem(meshName, "Game", Ogre::SCENE_DYNAMIC);
  item->setCastShadows(true);
  attach(item);
}
