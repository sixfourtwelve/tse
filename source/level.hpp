#pragma once

#include "gameobject.hpp"

#include <OgreSceneManager.h>

#include <memory>
#include <utility>
#include <vector>

class Level final {
public:
  explicit Level(Ogre::SceneManager *sceneManager);
  ~Level();

  void update(float dt) const;

  template <typename T, typename... Args>
  T *spawn(const Ogre::Vector3 &position, Args &&...args) {
    auto object = std::make_unique<T>(mSceneManager, mLevelNode,
                                      std::forward<Args>(args)...);
    T *result = object.get();
    result->getNode()->setPosition(position);
    mGameObjects.push_back(std::move(object));
    return result;
  }

private:
  Ogre::SceneManager *mSceneManager;
  Ogre::SceneNode *mLevelNode;
  std::vector<std::unique_ptr<GameObject>> mGameObjects;
};

