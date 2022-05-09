#version 460
#extension GL_GOOGLE_include_directive : enable
#include "common.glsl"

layout(location = 0) rayPayloadInEXT HitPayload payLoad;

void main()
{
    payLoad.emission = vec3(0.0);
    payLoad.done = true;
}
