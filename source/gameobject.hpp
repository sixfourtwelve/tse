#pragma once

#include <OgreMovableObject.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <vector>

class GameObject {
public:
  GameObject(Ogre::SceneManager *sceneManager, Ogre::SceneNode *parent)
      : mSceneManager(sceneManager),
        mNode(parent->createChildSceneNode(Ogre::SCENE_DYNAMIC)) {}

  virtual ~GameObject() {
    for (Ogre::MovableObject *object : mObjects) {
      object->detachFromParent();
      mSceneManager->destroyMovableObject(object);
    }
    mNode->removeAndDestroyAllChildren();
    mSceneManager->destroySceneNode(mNode);
  }

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;

  virtual void update(float) {}

  Ogre::SceneNode *getNode() const { return mNode; }

protected:
  void attach(Ogre::MovableObject *object, Ogre::SceneNode *node = nullptr) {
    (node ? node : mNode)->attachObject(object);
    mObjects.push_back(object);
  }

  Ogre::SceneManager *mSceneManager;
  Ogre::SceneNode *mNode;

private:
  std::vector<Ogre::MovableObject *> mObjects;
};

