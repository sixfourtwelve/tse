#pragma once

#include "gameobject.hpp"
#include <OgreLight.h>
#include <OgreSceneManager.h>
#include <string>

class CLight final : public CGameObject
{
public:
    CLight(Ogre::SceneManager* scnMgr, Ogre::SceneNode* parentNode, const std::string& name,
        const Ogre::Light::LightTypes type = Ogre::Light::LT_POINT);
    void Update(float dt) override;
};
