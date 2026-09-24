#version 150

in vec4 vertex;
in vec3 normal;
in vec2 uv0;

uniform mat4 worldViewProj;
uniform mat4 worldMatrix;

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec2 vUv;

void main()
{
  vec4 worldPos = worldMatrix * vertex;
  vWorldPos = worldPos.xyz;

  vWorldNormal = mat3(worldMatrix) * normal;

  vUv = uv0;
  gl_Position = worldViewProj * vertex;
}
