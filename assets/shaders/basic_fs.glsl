#version 150

uniform vec4 lightPos;
uniform vec4 lightColour;
uniform vec4 ambient;
uniform vec4 surfaceColour;

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

  vec3 lit = base * (ambient.rgb + lightColour.rgb * ndotl);
  fragColour = vec4(lit, 1.0);
}
