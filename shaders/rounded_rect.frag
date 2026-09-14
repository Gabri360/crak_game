#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec4 uColor;
uniform vec2 uSize;
uniform float uRadius;
uniform vec4 uBorderColor;
uniform float uBorderWidth;


float roundedBoxSDF(vec2 p, vec2 halfSize, float radius) {
    vec2 q = abs(p) - halfSize + radius;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

void main() {
    vec2 pixelPos = (vUV - 0.5) * uSize;
    vec2 halfSize = uSize * 0.5;

    float dist = roundedBoxSDF(pixelPos, halfSize, uRadius);

    float shapeAlpha = 1.0 - smoothstep(-1.0, 1.0, dist);
    float fillAlpha = 1.0 - smoothstep(-1.0, 1.0, dist + uBorderWidth);

    vec3 finalColor = mix(uBorderColor.rgb, uColor.rgb, fillAlpha);
    float finalAlphaMix = mix(uBorderColor.a, uColor.a, fillAlpha);

    FragColor = vec4(finalColor, finalAlphaMix * shapeAlpha);
}
