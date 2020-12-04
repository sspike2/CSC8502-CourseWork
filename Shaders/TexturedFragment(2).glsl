#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;

in Vertex { vec2 texCoord; }
IN;

out vec4 fragColour;
void main(void) {

  vec4 col = texture(diffuseTex, IN.texCoord);

  fragColour = col + texture(diffuseTex2, IN.texCoord);
}