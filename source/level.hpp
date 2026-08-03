#pragma once

#include "gameobject.hpp"
#include "light.hpp"
#include <OgreBullet.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreTexture.h>
#include <memory>
#include <vector>

class CLevel final
{
public:
    CLevel(Ogre::SceneManager* scnMgr, const std::string& name);
    ~CLevel();

    void Update(float dt) const;

    template <typename T, typename... Args>
    T* Spawn(const Ogre::Vector3& pos, Args&&... args)
    {
        auto obj = std::make_unique<T>(mScnMgr, mLevelNode, std::forward<Args>(args)...);
        T* raw = obj.get();
        raw->GetNode()->setPosition(pos);
        mGameObjects.push_back(std::move(obj));
        return raw;
    }

private:
    Ogre::SceneManager* mScnMgr;
    Ogre::SceneNode* mLevelNode;

    Ogre::Camera* mMainCam = nullptr;

    std::vector<std::unique_ptr<CGameObject>> mGameObjects;
    CLight* mLight;
};
