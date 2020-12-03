#version 330 core

uniform sampler2D diffuseTex; // Diffuse texture map
uniform sampler2D bumpTex;    // Bump map
uniform sampler2D emiisionTex;

uniform vec4 emissionColor;

uniform vec4 fogcolor;
uniform float linearStart;
uniform float linearEnd;
uniform float density;

uniform float fogType;

in Vertex {
  vec4 colour;
  vec2 texCoord;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
  vec4 ioEyeSpacePosition;
}
IN;

out vec4 fragColour[4]; // Our final outputted colours !

void main(void) {
  mat3 TBN =
      mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

  vec3 normal = texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;
  normal = normalize(TBN * normalize(normal));

  vec4 diffuse = texture2D(diffuseTex, IN.texCoord);
  vec4 emiss = texture2D(emiisionTex, IN.texCoord);

  float fogCoordinate = abs(IN.ioEyeSpacePosition.z / IN.ioEyeSpacePosition.w);
  float result = 0;

  if (fogType == 0) { // linear
    float foglength = linearEnd - linearStart;
    result = (linearEnd - fogCoordinate) / foglength;
  } else if (fogType == 1) { // exponential
    result = exp(-density * fogCoordinate);
  } else { // no fog
    result = 1;
  }

  result = 1.0 - clamp(result, 0.0, 1.0);

  // vec4 fog = mix(diffuse, fogcolor, result);

  vec4 fog = fogcolor * result;
  fog.a = 1;

  fragColour[0] = diffuse;
  fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
  fragColour[2] = emiss * emissionColor;
  fragColour[3] = fog;
}