#include "light.hpp"

#include <OgreMath.h>

Light::Light(Ogre::SceneManager *sceneManager, Ogre::SceneNode *parent)
    : GameObject(sceneManager, parent) {
  mLight = sceneManager->createLight();
  mLight->setType(Ogre::Light::LT_DIRECTIONAL);
  mLight->setPowerScale(Ogre::Math::PI);
  mLight->setDiffuseColour(1.0f, 0.92f, 0.78f);
  mLight->setSpecularColour(1.0f, 0.92f, 0.78f);
  mLight->setDirection(Ogre::Vector3(-0.65f, -1.0f, -0.35f).normalisedCopy());
  attach(mLight);
}

void Light::update(const float dt) {
  mAngle += dt * 0.08f;
  mLight->setDirection(
      Ogre::Vector3(Ogre::Math::Sin(mAngle) * 0.65f, -1.0f,
                    Ogre::Math::Cos(mAngle) * 0.65f)
          .normalisedCopy());
}
