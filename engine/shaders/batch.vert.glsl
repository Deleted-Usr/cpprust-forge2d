#version 450 core

layout(location = 0) in vec2  aPosition;
layout(location = 1) in vec2  aUV;
layout(location = 2) in vec4  aColour;
layout(location = 3) in float aTextureSlot;

layout(std140, binding = 0) uniform ViewData
{
    mat4 uViewProjection;
};

out vec2 vUV;
out vec4 vColour;
flat out int vTextureSlot;

void main()
{
    vUV          = aUV;
    vColour      = aColour;
    vTextureSlot = int(aTextureSlot);

    gl_Position = uViewProjection * vec4(aPosition, 0.0, 1.0);
}
