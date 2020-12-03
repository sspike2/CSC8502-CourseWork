#version 330 core
#define PI 3.1415926535897932384626433832795
uniform sampler2D diffuseTex;

in Vertex { vec2 texCoord; }
IN;

out vec4 fragColour[2];
void main(void) {
  float aperture = 178.0;
  float apertureHalf = 0.5 * aperture * (PI / 180.0);
  float maxFactor = sin(apertureHalf);

  vec2 uv;
  vec2 xy = 2.0 * IN.texCoord.xy - 1.0;
  float d = length(xy);
  if (d < (2.0 - maxFactor)) {
    d = length(xy * maxFactor);
    float z = sqrt(1.0 - d * d);
    float r = atan(d, z) / PI;
    float phi = atan(xy.y, xy.x);

    uv.x = r * cos(phi) + 0.5;
    uv.y = r * sin(phi) + 0.5;
  } else {
    uv = IN.texCoord.xy;
  }
  vec4 c = texture2D(diffuseTex, uv);
  fragColour[0] = c;
  fragColour[1] = c;
}