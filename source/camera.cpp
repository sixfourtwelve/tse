#include "camera.hpp"
#include <OgreInput.h>
#include <OgreLog.h>
#include <OgreLogManager.h>
#include <OgreSceneManager.h>
#include <SDL2/SDL_scancode.h>
#include <string>

Camera::Camera(const std::string &name, Ogre::SceneManager *scnMgr)
    : GameObject(scnMgr, scnMgr->getRootSceneNode()),
      mCamera(scnMgr->createCamera(name)) {
  mNode->setPosition(0, 5, 20);
  mCamera->setNearClipDistance(1);
  mCamera->setAutoAspectRatio(true);
  mNode->attachObject(mCamera);

  mCamMan = std::make_unique<OgreBites::CameraMan>(mNode);
  mCamMan->setStyle(OgreBites::CameraStyle::CS_FREELOOK);
  mCamMan->setTopSpeed(10.f);
}

void Camera::update(float dt) {}
