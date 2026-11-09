#version 450 core

in vec2 vUV;
in vec4 vColour;
flat in int vTextureSlot;

uniform sampler2D uTextures[8];
layout(location = 0) out vec4 outColour;

void main()
{
    vec2 dx = dFdx(vUV);
    vec2 dy = dFdy(vUV);

    vec4 sampled;

    // Constant sampler indices support mixed textures in a single draw.
    switch (vTextureSlot)
    {
        case 0: sampled = textureGrad(uTextures[0], vUV, dx, dy); break;
        case 1: sampled = textureGrad(uTextures[1], vUV, dx, dy); break;
        case 2: sampled = textureGrad(uTextures[2], vUV, dx, dy); break;
        case 3: sampled = textureGrad(uTextures[3], vUV, dx, dy); break;
        case 4: sampled = textureGrad(uTextures[4], vUV, dx, dy); break;
        case 5: sampled = textureGrad(uTextures[5], vUV, dx, dy); break;
        case 6: sampled = textureGrad(uTextures[6], vUV, dx, dy); break;
        case 7: sampled = textureGrad(uTextures[7], vUV, dx, dy); break;

        default: sampled = vec4(1, 0, 1, 1); break;
    }

    outColour = sampled * vColour;
}
