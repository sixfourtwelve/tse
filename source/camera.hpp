#pragma once

#include "gameobject.hpp"
#include <OgreCameraMan.h>
#include <OgreInput.h>
#include <string>

class CAmera final : public CGameObject, public OgreBites::InputListener
{
public:
    CAmera(const std::string& name, Ogre::SceneManager* scnMgr);

    Ogre::Camera* GetCamera(void) const { return mCamera; }
    OgreBites::CameraMan* GetCameraMan(void) const { return mCamMan.get(); }

    void Update(float dt) override;

private:
    Ogre::Camera* mCamera;
    std::unique_ptr<OgreBites::CameraMan> mCamMan;
};
