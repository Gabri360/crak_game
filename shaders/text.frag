#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D uText;
uniform vec4 uColor;

uniform vec4 uOutlineColor;
uniform float uOutlineWidth;

void main()
{
  vec2 texelSize = 1.0 / vec2(textureSize(uText, 0));

  float glyphAlpha = texture(uText, TexCoords).r;

  float outlineAlpha = 0.0;
  const int SAMPLES = 16;
  for (int i = 0; i < SAMPLES; i++) {
    float angle = float(i) / float(SAMPLES) * 6.28318530718; // 2*PI
    vec2 offset = vec2(cos(angle), sin(angle)) * texelSize * uOutlineWidth;
    outlineAlpha = max(outlineAlpha, texture(uText, TexCoords + offset).r);
  }

  vec3 finalColor = mix(uOutlineColor.rgb, uColor.rgb, glyphAlpha);
  float finalAlpha = max(glyphAlpha, outlineAlpha);

  FragColor = vec4(finalColor, finalAlpha * mix(uOutlineColor.a, uColor.a, glyphAlpha));
}
