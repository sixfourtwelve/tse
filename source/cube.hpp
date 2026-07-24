#pragma once

#include "gameobject.hpp"
#include <OgreColourValue.h>
#include <OgreVector.h>
#include <optional>

// A cube using the custom BasicCube GLSL material.
//
// All cubes share ONE unit-cube mesh (built once, then reused with per-node
// scaling) — this keeps geometry to a single vertex buffer. Pass an optional
// colour to tint an individual cube (clones the material and sets the
// surfaceColour shader uniform).
class Cube final : public GameObject {
public:
  Cube(Ogre::SceneManager *scnMgr, Ogre::SceneNode *parent,
       std::optional<Ogre::ColourValue> colour = std::nullopt,
       const Ogre::Vector3 &size = Ogre::Vector3(2, 2, 2));

private:
  // Builds the shared 1x1x1 mesh on first use; returns its name.
  static Ogre::String ensureMesh(Ogre::SceneManager *scnMgr);
};
