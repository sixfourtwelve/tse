#version 150

// -----------------------------------------------------------------------------
// Vertex shader for the BasicCube material.
//
// The input names below are NOT arbitrary: Ogre binds vertex buffer data to
// GLSL inputs BY NAME using a fixed table (see OgreGLSLProgramCommon.cpp).
// Use these reserved names or the data won't arrive:
//   vertex/position, normal, colour, secondary_colour,
//   uv0..uv7, tangent, binormal, blendWeights, blendIndices
// -----------------------------------------------------------------------------

in vec4 vertex; // object-space position (VES_POSITION)
in vec3 normal; // object-space normal   (VES_NORMAL)
in vec2 uv0; // texcoord set 0        (VES_TEXTURE_COORDINATES)

// Filled automatically by Ogre via param_named_auto in basic.program.
uniform mat4 worldViewProj; // model-view-projection
uniform mat4 worldMatrix; // object -> world

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec2 vUv;

void main()
{
  vec4 worldPos = worldMatrix * vertex;
  vWorldPos = worldPos.xyz;

  // mat3(worldMatrix) is correct for uniform scaling (our cubes).
  // For non-uniform scale you'd pass an inverse-transpose normal matrix.
  vWorldNormal = mat3(worldMatrix) * normal;

  vUv = uv0;
  gl_Position = worldViewProj * vertex;
}
