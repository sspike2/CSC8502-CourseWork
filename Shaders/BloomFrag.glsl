#version 330 core

uniform sampler2D emissionTex;
uniform sampler2D diffuseTex;
uniform sampler2D lightTex;

uniform int isVertical;

// uniform vec4 emissionColor;
in Vertex { vec2 texCoord; }
IN;

out vec4 fragColor;

const float scaleFactors[7] =
    float[](0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006);

void main(void) {
  vec4 bloom = vec4(0, 0, 0, 1);
  vec2 delta = vec2(0, 0);
  vec4 diffuse = texture(diffuseTex, IN.texCoord);

  if (isVertical == 1) {
    delta = dFdy(IN.texCoord);
  } else {
    delta = dFdx(IN.texCoord);
  }
  for (int i = 0; i < 7; i++) {
    vec2 offset = delta * (i - 3);
    vec4 tmp = texture2D(emissionTex, IN.texCoord.xy + offset);
    vec4 light = texture2D(lightTex, IN.texCoord.xy + offset);
    tmp += light * 0.2;

    bloom += tmp * scaleFactors[i];
  }

  fragColor = diffuse + bloom;
}