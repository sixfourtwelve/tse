#pragma once

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

class CGameObject
{
public:
    CGameObject(Ogre::SceneManager* scnMgr, Ogre::SceneNode* parent)
        : mScnMgr(scnMgr)
        , mNode(parent->createChildSceneNode())
    {
    }

    virtual ~CGameObject()
    {
        mNode->destroyAllObjects();
        mScnMgr->destroySceneNode(mNode);
    }

    virtual void Update(float) {}
    Ogre::SceneNode* GetNode() const { return mNode; }

protected:
    Ogre::SceneManager* mScnMgr;
    Ogre::SceneNode* mNode;
};
