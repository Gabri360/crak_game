#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec4 uColor;
uniform vec2 uOrigin;
uniform vec2 uSize;
uniform vec2 uPointA;
uniform vec2 uPointB;
uniform vec2 uPointC;
uniform float uRadius;

float sdTriangle(vec2 p, vec2 a, vec2 b, vec2 c) {
    vec2 e0 = b - a, e1 = c - b, e2 = a - c;
    vec2 v0 = p - a, v1 = p - b, v2 = p - c;

    vec2 pq0 = v0 - e0 * clamp(dot(v0, e0) / dot(e0, e0), 0.0, 1.0);
    vec2 pq1 = v1 - e1 * clamp(dot(v1, e1) / dot(e1, e1), 0.0, 1.0);
    vec2 pq2 = v2 - e2 * clamp(dot(v2, e2) / dot(e2, e2), 0.0, 1.0);

    float s = sign(e0.x * e2.y - e0.y * e2.x);

    vec2 d0 = vec2(dot(pq0, pq0), s * (v0.x * e0.y - v0.y * e0.x));
    vec2 d1 = vec2(dot(pq1, pq1), s * (v1.x * e1.y - v1.y * e1.x));
    vec2 d2 = vec2(dot(pq2, pq2), s * (v2.x * e2.y - v2.y * e2.x));

    vec2 d = min(min(d0, d1), d2);

    return -sqrt(d.x) * sign(d.y);
}

void main() {
    vec2 pixelPos = uOrigin + vUV * uSize;

    float dist = sdTriangle(pixelPos, uPointA, uPointB, uPointC) - uRadius;

    float alpha = 1.0 - smoothstep(-1.0, 1.0, dist);

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}
