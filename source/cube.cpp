#include "cube.hpp"

#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsPbsDatablock.h>
#include <OgreItem.h>
#include <OgreManualObject2.h>
#include <OgreMeshManager2.h>
#include <OgreRoot.h>
#include <OgreStringConverter.h>

namespace {
constexpr const char *kMeshName = "Tse/UnitCube";

void addQuad(Ogre::ManualObject *object, const Ogre::Vector3 &a,
             const Ogre::Vector3 &b, const Ogre::Vector3 &c,
             const Ogre::Vector3 &d, const Ogre::Vector3 &normal,
             Ogre::uint32 &base) {
  object->position(a);
  object->normal(normal);
  object->textureCoord(0.0f, 0.0f);
  object->position(b);
  object->normal(normal);
  object->textureCoord(1.0f, 0.0f);
  object->position(c);
  object->normal(normal);
  object->textureCoord(1.0f, 1.0f);
  object->position(d);
  object->normal(normal);
  object->textureCoord(0.0f, 1.0f);
  object->triangle(base, base + 1u, base + 2u);
  object->triangle(base, base + 2u, base + 3u);
  base += 4u;
}

Ogre::HlmsPbsDatablock *createDatablock(const Ogre::ColourValue &colour) {
  static Ogre::uint32 counter = 0u;
  const Ogre::String name =
      "Tse/Cube/" + Ogre::StringConverter::toString(counter++);

  auto *hlms = static_cast<Ogre::HlmsPbs *>(
      Ogre::Root::getSingleton().getHlmsManager()->getHlms(Ogre::HLMS_PBS));
  auto *datablock = static_cast<Ogre::HlmsPbsDatablock *>(
      hlms->createDatablock(name, name, Ogre::HlmsMacroblock(),
                            Ogre::HlmsBlendblock(), Ogre::HlmsParamVec()));
  datablock->setDiffuse(Ogre::Vector3(colour.r, colour.g, colour.b));
  datablock->setRoughness(0.55f);
  datablock->setFresnel(Ogre::Vector3(0.04f), false);
  return datablock;
}
} // namespace

Ogre::String Cube::ensureMesh(Ogre::SceneManager *sceneManager,
                              const Ogre::String &datablockName) {
  if (Ogre::MeshManager::getSingleton().getByName(kMeshName, "Game"))
    return kMeshName;

  constexpr float h = 0.5f;
  const Ogre::Vector3 p000(-h, -h, -h), p100(h, -h, -h),
      p110(h, h, -h), p010(-h, h, -h), p001(-h, -h, h),
      p101(h, -h, h), p111(h, h, h), p011(-h, h, h);

  Ogre::ManualObject *object =
      sceneManager->createManualObject(Ogre::SCENE_DYNAMIC);
  object->estimateVertexCount(24u);
  object->estimateIndexCount(36u);
  object->begin(datablockName, Ogre::OT_TRIANGLE_LIST);

  Ogre::uint32 base = 0u;
  addQuad(object, p001, p101, p111, p011, Ogre::Vector3::UNIT_Z, base);
  addQuad(object, p100, p000, p010, p110, Ogre::Vector3::NEGATIVE_UNIT_Z,
          base);
  addQuad(object, p101, p100, p110, p111, Ogre::Vector3::UNIT_X, base);
  addQuad(object, p000, p001, p011, p010, Ogre::Vector3::NEGATIVE_UNIT_X,
          base);
  addQuad(object, p011, p111, p110, p010, Ogre::Vector3::UNIT_Y, base);
  addQuad(object, p000, p100, p101, p001, Ogre::Vector3::NEGATIVE_UNIT_Y,
          base);

  object->end();
  object->convertToMesh(kMeshName, "Game");
  sceneManager->destroyManualObject(object);
  return kMeshName;
}

Cube::Cube(Ogre::SceneManager *sceneManager, Ogre::SceneNode *parent,
           std::optional<Ogre::ColourValue> colour, const Ogre::Vector3 &size)
    : GameObject(sceneManager, parent) {
  Ogre::HlmsPbsDatablock *datablock =
      createDatablock(colour.value_or(Ogre::ColourValue(0.75f, 0.75f, 0.78f)));
  Ogre::Item *item = sceneManager->createItem(
      ensureMesh(sceneManager, *datablock->getNameStr()), "Game",
      Ogre::SCENE_DYNAMIC);
  item->setDatablock(datablock);
  attach(item);
  mNode->setScale(size);
}

