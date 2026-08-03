#include "light.hpp"

CLight::CLight(Ogre::SceneManager* scnMgr, Ogre::SceneNode* parentNode, const std::string& name,
    const Ogre::Light::LightTypes type)
    : CGameObject(scnMgr, parentNode)
{
    Ogre::Light* light = mScnMgr->createLight(name);
    light->setType(type);
    light->setShadowNearClipDistance(1);
    mNode->attachObject(light);
}

void CLight::Update(const float dt)
{
    mNode->yaw(Ogre::Degree(100) * dt);
}
