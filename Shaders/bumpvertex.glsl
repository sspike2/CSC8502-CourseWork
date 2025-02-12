#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent; // New ! Note , Vec4 !
in vec2 texCoord;

out Vertex {
  vec4 colour;
  vec2 texCoord;
  vec3 normal;
  vec3 tangent;  // New ! Note , Vec3 !
  vec3 binormal; // New !
  vec3 worldPos;
  vec4 ioEyeSpacePosition;
}
OUT;

void main(void) {
  OUT.colour = colour;
  OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

  mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

  vec3 wNormal = normalize(normalMatrix * normalize(normal));
  vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

  OUT.normal = wNormal;
  OUT.tangent = wTangent;
  OUT.binormal = cross(wNormal, wTangent) * tangent.w;

  vec4 worldPos = (modelMatrix * vec4(position, 1));

  OUT.worldPos = worldPos.xyz;

  mat4 mvMatrix = viewMatrix * modelMatrix;
  OUT.ioEyeSpacePosition = mvMatrix * vec4(position, 1.0);
  
  gl_Position = (projMatrix * viewMatrix) * worldPos;
}