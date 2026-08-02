#include "light.hpp"

#include <OgreMath.h>

Light::Light(Ogre::SceneManager *sceneManager, Ogre::SceneNode *parent)
    : GameObject(sceneManager, parent) {
  Ogre::Light *light = sceneManager->createLight();
  light->setType(Ogre::Light::LT_POINT);
  light->setPowerScale(Ogre::Math::PI * 8.0f);
  light->setDiffuseColour(1.0f, 0.92f, 0.78f);
  light->setSpecularColour(1.0f, 0.92f, 0.78f);
  light->setAttenuationBasedOnRadius(30.0f, 0.01f);
  attach(light);
  mNode->setPosition(0.0f, 10.0f, 15.0f);
}

void Light::update(const float dt) {
  mAngle += dt * 0.55f;
  mNode->setPosition(Ogre::Math::Sin(mAngle) * 15.0f, 10.0f,
                     Ogre::Math::Cos(mAngle) * 15.0f);
}

