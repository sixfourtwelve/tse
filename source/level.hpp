#pragma once

#include "gameobject.hpp"
#include "light.hpp"
#include <OgreBullet.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreTexture.h>
#include <memory>
#include <vector>

class Level final {
public:
  Level(Ogre::SceneManager *scnMgr, const std::string &name);
  ~Level();

  void update(float dt) const;

  template <typename T, typename... Args>
  T *spawn(const Ogre::Vector3 &pos, Args &&...args) {
    auto obj =
        std::make_unique<T>(mScnMgr, mLevelNode, std::forward<Args>(args)...);
    T *raw = obj.get();
    raw->getNode()->setPosition(pos);
    mGameObjects.push_back(std::move(obj));
    return raw;
  }

private:
  Ogre::SceneManager *mScnMgr;
  Ogre::SceneNode *mLevelNode;

  Ogre::Camera *mMainCam = nullptr;

  std::vector<std::unique_ptr<GameObject>> mGameObjects;
  Light *mLight;
};
