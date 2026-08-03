#pragma once

#include "gameobject.hpp"
#include <OgreColourValue.h>
#include <OgreVector.h>
#include <optional>

class CCube final : public CGameObject
{
public:
    CCube(Ogre::SceneManager* scnMgr, Ogre::SceneNode* parent, std::optional<Ogre::ColourValue> colour = std::nullopt,
        const Ogre::Vector3& size = Ogre::Vector3(2, 2, 2));

    void Update(float dt) override;

private:
    static Ogre::String EnsureMesh(Ogre::SceneManager* scnMgr);
};
