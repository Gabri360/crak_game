#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform float ut;
uniform vec4 uColorTop;
uniform vec4 uColorBottom;

void main() {
      FragColor = mix(uColorBottom, uColorTop, vUV.y + sin(vUV.x + ut) * 0.1);
}
