#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 0) uniform UniformBufferObject
{
    mat4    model;
    mat4    view;
    mat4    proj;
    vec4    color;
} ubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 colorMod;
//MVP is the same except we never move the sky

void main()
{
    mat3 matrix;
    mat4 mvp = ubo.proj * ubo.view * ubo.model;

    //positions
    matrix = mat3(mvp);
    gl_Position =  vec4(matrix * inPosition, 1.0); //I had the vec4 * matrix?
   
    //pass throughs
    colorMod = ubo.color;
    fragTexCoord = inTexCoord;

}