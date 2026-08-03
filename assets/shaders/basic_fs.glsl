#version 150

// -----------------------------------------------------------------------------
// Fragment shader for the BasicCube material.
// Simple single point-light diffuse + ambient, tinted by surfaceColour.
// -----------------------------------------------------------------------------

// Filled automatically by Ogre (see basic.program).
uniform vec4 lightPos; // world-space light position (w = 1 for point light)
uniform vec4 lightColour; // light 0 diffuse colour
uniform vec4 ambient; // scene ambient light colour
uniform vec4 surfaceColour; // per-material tint (overridable per cube from C++)

// To texture instead of flat colour:
//   1. uncomment the sampler below
//   2. add `uniform sampler2D diffuseTex;` binding in basic.program
//   3. add a texture_unit to the material pass
//   4. swap `base` to the texture() line
// uniform sampler2D diffuseTex;

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vUv;

out vec4 fragColour;

void main()
{
  vec3 n = normalize(vWorldNormal);
  vec3 l = normalize(lightPos.xyz - vWorldPos);
  float ndotl = max(dot(n, l), 0.0);

  vec3 base = surfaceColour.rgb;
  // vec3 base = texture(diffuseTex, vUv).rgb; // <- swap in for texturing

  vec3 lit = base * (ambient.rgb + lightColour.rgb * ndotl);
  fragColour = vec4(lit, 1.0);
}
