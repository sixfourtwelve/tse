#pragma once

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

class GameObject {
public:
  GameObject(Ogre::SceneManager *scnMgr, Ogre::SceneNode *parent)
      : mScnMgr(scnMgr), mNode(parent->createChildSceneNode()) {}

  virtual ~GameObject() {
    mNode->destroyAllObjects();
    mScnMgr->destroySceneNode(mNode);
  }

  virtual void update(float) {}
  Ogre::SceneNode *getNode() const { return mNode; }

protected:
  Ogre::SceneManager *mScnMgr;
  Ogre::SceneNode *mNode;
};
