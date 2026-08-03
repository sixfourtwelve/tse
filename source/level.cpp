#include "level.hpp"
#include "cube.hpp"
#include "light.hpp"
#include <OgreBullet.h>
#include <OgreColourValue.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreMath.h>
#include <OgrePrerequisites.h>
#include <OgreRenderTarget.h>
#include <OgreTextureManager.h>
#include <OgreViewport.h>

CLevel::CLevel(Ogre::SceneManager* scnMgr, const std::string& name)
    : mScnMgr(scnMgr)
{
    mLevelNode = mScnMgr->getRootSceneNode()->createChildSceneNode(name);

    scnMgr->setAmbientLight(Ogre::ColourValue(0.19, 0.19, 0.19, 1));

    scnMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
    scnMgr->setShadowTexturePixelFormat(Ogre::PF_DEPTH16);
    scnMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));
    scnMgr->setShadowTextureSize(8192);
    scnMgr->setShadowTextureCount(1);
    scnMgr->setShadowFarDistance(10000);

    mLight = Spawn<CLight>(Ogre::Vector3(0, 10, 15), "MainLight", Ogre::Light::LT_POINT);

    Spawn<CCube>({ 0, 0, 0 });
    Spawn<CCube>({ 4, 0, 0 }, Ogre::ColourValue(0.2, 0.6, 1.0)); // blue
    Spawn<CCube>({ -4, 0, 0 }, Ogre::ColourValue(0.9, 0.7, 0.2)); // amber
}

CLevel::~CLevel()
{
    mGameObjects.clear();
    mScnMgr->destroySceneNode(mLevelNode);
}

void CLevel::Update(const float dt) const
{
    if (mGameObjects.empty())
        return;

    for (const auto& obj : mGameObjects)
        obj->Update(dt);
}
