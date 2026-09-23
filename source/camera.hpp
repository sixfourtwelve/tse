#pragma once

#include "gameobject.hpp"
#include <OgreCameraMan.h>
#include <OgreInput.h>
#include <string>

class Camera final : public GameObject, public OgreBites::InputListener
{
public:
    Camera(const std::string& name, Ogre::SceneManager* scnMgr);

    Ogre::Camera* GetCamera(void) const { return mCamera; }
    OgreBites::CameraMan* GetCameraMan(void) const { return mCamMan.get(); }

    void Update(float dt) override;

private:
    Ogre::Camera* mCamera;
    std::unique_ptr<OgreBites::CameraMan> mCamMan;
};
