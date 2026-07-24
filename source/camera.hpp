#pragma once

#include "gameobject.hpp"
#include <OgreCameraMan.h>
#include <OgreInput.h>
#include <string>

class Camera final : public GameObject, public OgreBites::InputListener {
public:
  Camera(const std::string &name, Ogre::SceneManager *scnMgr);

  Ogre::Camera *getCamera(void) const { return mCamera; }
  OgreBites::CameraMan *getCameraMan(void) const { return mCamMan.get(); }

  void update(float dt) override;

private:
  Ogre::Camera *mCamera;
  std::unique_ptr<OgreBites::CameraMan> mCamMan;
};
