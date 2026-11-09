#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D uTexture;
uniform vec4 uTint;

void main()
{
    FragColor = texture(uTexture, texCoord) * uTint;
}
