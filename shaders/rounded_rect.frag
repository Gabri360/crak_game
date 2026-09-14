#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec4 uColor;
uniform vec2 uSize;
uniform float uRadius;


float roundedBoxSDF(vec2 p, vec2 halfSize, float radius) {
    vec2 q = abs(p) - halfSize + radius;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

void main() {
    vec2 pixelPos = (vUV - 0.5) * uSize;
    vec2 halfSize = uSize * 0.5;

    float dist = roundedBoxSDF(pixelPos, halfSize, uRadius);

    float alpha = 1.0 - smoothstep(-1.0, 1.0, dist);

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}
