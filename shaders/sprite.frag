#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;

void main() {
    if (uUseTexture)
        FragColor = texture(uTexture, vUV) * uColor;
    else
        FragColor = uColor;
}