#include "cube.hpp"
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <OgrePass.h>
#include <OgreTechnique.h>
#include <string>

namespace
{
    constexpr const char* kMeshName = "UnitCube";
    constexpr const char* kMaterial = "BasicCube";

    void AddQuad(Ogre::ManualObject* mo, const Ogre::Vector3& a, const Ogre::Vector3& b, const Ogre::Vector3& c,
        const Ogre::Vector3& d, const Ogre::Vector3& n, unsigned& base)
    {
        mo->position(a);
        mo->normal(n);
        mo->textureCoord(0, 0);
        mo->position(b);
        mo->normal(n);
        mo->textureCoord(1, 0);
        mo->position(c);
        mo->normal(n);
        mo->textureCoord(1, 1);
        mo->position(d);
        mo->normal(n);
        mo->textureCoord(0, 1);
        mo->triangle(base, base + 1, base + 2);
        mo->triangle(base, base + 2, base + 3);
        base += 4;
    }
} // namespace

Ogre::String CCube::EnsureMesh(Ogre::SceneManager* scnMgr)
{
    if (Ogre::MeshManager::getSingleton().getByName(kMeshName))
        return kMeshName;

    constexpr float h = 0.5f; // unit cube, centred on the origin
    const Ogre::Vector3 p000(-h, -h, -h), p100(h, -h, -h), p110(h, h, -h), p010(-h, h, -h), p001(-h, -h, h),
        p101(h, -h, h), p111(h, h, h), p011(-h, h, h);

    auto* mo = scnMgr->createManualObject();
    mo->begin(kMaterial, Ogre::RenderOperation::OT_TRIANGLE_LIST, "Game");

    unsigned base = 0;
    AddQuad(mo, p001, p101, p111, p011, Ogre::Vector3(0, 0, 1), base); // +Z
    AddQuad(mo, p100, p000, p010, p110, Ogre::Vector3(0, 0, -1),
        base); // -Z
    AddQuad(mo, p101, p100, p110, p111, Ogre::Vector3(1, 0, 0), base); // +X
    AddQuad(mo, p000, p001, p011, p010, Ogre::Vector3(-1, 0, 0),
        base); // -X
    AddQuad(mo, p011, p111, p110, p010, Ogre::Vector3(0, 1, 0), base); // +Y
    AddQuad(mo, p000, p100, p101, p001, Ogre::Vector3(0, -1, 0),
        base); // -Y

    mo->end();
    mo->convertToMesh(kMeshName);
    scnMgr->destroyManualObject(mo);
    return kMeshName;
}

CCube::CCube(Ogre::SceneManager* scnMgr, Ogre::SceneNode* parent, std::optional<Ogre::ColourValue> colour,
    const Ogre::Vector3& size)
    : CGameObject(scnMgr, parent)
{
    const Ogre::String meshName = EnsureMesh(scnMgr);
    Ogre::Entity* ent = mScnMgr->createEntity(meshName);

    Ogre::String matName = kMaterial;
    if (colour)
    {
        static int counter = 0;
        auto baseMat = Ogre::MaterialManager::getSingleton().getByName(kMaterial, "Game");
        auto mat = baseMat->clone(kMaterial + std::string("#") + std::to_string(counter++));
        mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("surfaceColour", *colour);
        matName = mat->getName();
    }

    ent->setMaterialName(matName);
    mNode->attachObject(ent);
    mNode->setScale(size);
    mNode->setOrientation(Ogre::Quaternion(Ogre::Degree(45), Ogre::Vector3::UNIT_Y)
        * Ogre::Quaternion(Ogre::Degree(30), Ogre::Vector3::UNIT_X));
}

void CCube::Update(const float dt)
{
    mNode->yaw(Ogre::Degree(20) * dt);
}
