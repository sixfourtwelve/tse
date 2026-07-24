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

Level::Level(Ogre::SceneManager *scnMgr, const std::string &name)
    : mScnMgr(scnMgr) {
  mLevelNode = mScnMgr->getRootSceneNode()->createChildSceneNode(name);

  scnMgr->setAmbientLight(Ogre::ColourValue(0.19, 0.19, 0.19, 1));

  scnMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
  scnMgr->setShadowTexturePixelFormat(Ogre::PF_DEPTH16);
  scnMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));
  scnMgr->setShadowTextureSize(8192);
  scnMgr->setShadowTextureCount(1);
  scnMgr->setShadowFarDistance(10000);

  mLight = spawn<Light>(Ogre::Vector3(0, 10, 15), "MainLight",
                        Ogre::Light::LT_POINT);

  // Demo: three cubes sharing one mesh + custom GLSL material.
  spawn<Cube>({0, 0, 0});                                    // default tint
  spawn<Cube>({4, 0, 0}, Ogre::ColourValue(0.2, 0.6, 1.0));  // blue
  spawn<Cube>({-4, 0, 0}, Ogre::ColourValue(0.9, 0.7, 0.2)); // amber
}

Level::~Level() {
  mGameObjects.clear();
  mScnMgr->destroySceneNode(mLevelNode);
}

void Level::update(const float dt) const {
  if (mGameObjects.empty())
    return;

  for (const auto &obj : mGameObjects)
    obj->update(dt);
}
