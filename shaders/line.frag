#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec4 uColor;
uniform vec2 uOrigin;   // angolo in alto a sinistra del quad, in pixel-schermo
uniform vec2 uSize;     // dimensioni del quad, in pixel
uniform vec2 uPointA;   // punto di inizio della linea, in pixel-schermo
uniform vec2 uPointB;   // punto di fine della linea, in pixel-schermo
uniform float uRadius;  // meta' dello spessore della linea

// Distanza con segno da un segmento (capsula: rettangolo + estremi arrotondati)
float sdSegment(vec2 p, vec2 a, vec2 b) {
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

void main() {
    vec2 pixelPos = uOrigin + vUV * uSize;

    float dist = sdSegment(pixelPos, uPointA, uPointB) - uRadius;

    float alpha = 1.0 - smoothstep(-1.0, 1.0, dist);

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}
