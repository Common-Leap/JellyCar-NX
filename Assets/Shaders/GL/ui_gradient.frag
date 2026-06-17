#version 330 core
in vec2 localCoord;

uniform vec4 topColor;
uniform vec4 bottomColor;

out vec4 color;

void main()
{
    color = mix(bottomColor, topColor, localCoord.y);
}
