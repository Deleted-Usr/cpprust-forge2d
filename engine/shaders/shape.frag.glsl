#version 460 core
out vec4 FragColor;

in vec4 vertexColour;
uniform vec4 customColour;

void main()
{
    FragColor = customColour;
}