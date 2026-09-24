#include "level.hpp"
#include <OgreBullet.h>
#include <OgreColourValue.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreLight.h>
#include <OgreMaterialManager.h>
#include <OgreMath.h>
#include <OgrePass.h>
#include <OgrePrerequisites.h>
#include <OgreRenderTarget.h>
#include <OgreResourceGroupManager.h>
#include <OgreTextureManager.h>
#include <OgreViewport.h>

namespace
{
    void configureShadowCasterBias()
    {
        auto caster = Ogre::MaterialManager::getSingleton().getByName("Ogre/TextureShadowCaster");
        if (!caster)
            return;

        auto* pass = caster->getTechniques().back()->getPass(0);
        pass->setDepthBias(-1.0f, -4.0f);
    }

    void configureBlenderLight(Ogre::SceneManager& sceneManager, const Ogre::String& name)
    {
        if (!sceneManager.hasLight(name))
            return;

        auto* light = sceneManager.getLight(name);
        const auto type = light->getType();

        if (type != Ogre::Light::LT_POINT)
        {
            auto* lightNode = light->getParentSceneNode();
            lightNode->rotate(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X), Ogre::Node::TS_LOCAL);
        }

        if (type != Ogre::Light::LT_DIRECTIONAL)
        {
            const Ogre::Real range = light->getAttenuationRange();
            if (range > 0)
                light->setAttenuation(range, 1, 4.5f / range, 75.0f / (range * range));
        }
    }
} // namespace

Level::Level(Ogre::SceneManager* scnMgr, const std::string& name)
    : mScnMgr(scnMgr)
{
    mLevelNode = mScnMgr->getRootSceneNode()->createChildSceneNode(name);

    auto& resources = Ogre::ResourceGroupManager::getSingleton();
    resources.setWorldResourceGroupName("Game");

    scnMgr->setAmbientLight(Ogre::ColourValue(0.19, 0.19, 0.19, 1));

    scnMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
    scnMgr->setShadowTextureSettings(4096, 1, Ogre::PF_DEPTH32F);
    scnMgr->setShadowTextureSelfShadow(true);
    scnMgr->setShadowCasterRenderBackFaces(false);
    scnMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));

    scnMgr->setShadowFarDistance(150);
    scnMgr->setShadowDirLightTextureOffset(0.25f);
    configureShadowCasterBias();

    mLevelNode->loadChildren("test.scene");
    configureBlenderLight(*mScnMgr, "Light");

    if (mScnMgr->hasEntity("Cube"))
        mScnMgr->getEntity("Cube")->setCastShadows(false);
}

Level::~Level()
{
    mGameObjects.clear();
    mScnMgr->destroySceneNode(mLevelNode);
}

void Level::Update(const float dt) const
{
    if (mGameObjects.empty())
        return;

    for (const auto& obj : mGameObjects)
        obj->Update(dt);
}
