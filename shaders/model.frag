#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 worldPosition;
layout(location = 4) in vec4 cameraPos;
layout(location = 5) in vec4 lightPos;
layout(location = 6) in vec4 lightColor;

layout(location = 0) out vec4 outColor;


void main()
{
    //- +
    vec3 lightDir = normalize(worldPosition.xyz-lightPos.xyz);
    vec4 texColor = texture(texSampler, fragTexCoord);
    //vec4 temp;
    float intensity = max(dot(-lightDir,inNormal), 0.0);
    //did he un negative the lightdir

    
    if(intensity > .98){intensity = 1;}
    else if(intensity > .60){intensity = .8;}
    else if(intensity > .25){intensity =  .45;}
    else{intensity = .2;}

    texColor.xyz = texColor.xyz *intensity;
    
    outColor = texColor * colorMod;
}

/*
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 outNormal;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 worldPosition;
layout(location = 4) in vec4 cameraPos;



layout(location = 0) out vec4 outColor;


void main()
{
    
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = texColor * colorMod;

    Correct shadow
    texColor.xyz = texColor.xyz * max(.4,(dot(-lightDir,inNormal)));

    Wrong
        temp = texColor;
    temp.xyz = temp.xyz / .49;
    //temp.xyz *= .294;
    /*
    if(temp.x > 2){temp.x = 2;}
    else if(temp.x > 1){temp.x = 1;}
    else{temp.x = .5;}
    /////////////////////
    if(temp.y > 2){temp.y = 2;}
    else if(temp.y > 1){temp.y = 1;}
    else{temp.y = .5;}    
    /////////////////////
    if(temp.z > 2){temp.z = 2;}
    else if(temp.z > 1){temp.z = 1;}
    else{temp.z = .5;}
    /////////////////////
    //temp.xyz = (temp.xyz / 2) + .4;
    temp.xyz *= .294;

    if(intensity > 1){texColor.x = 1;texColor.y = 1;texColor.z = 1;}
    else if(intensity > .50){texColor.x = .75; texColor.y = .75; texColor.z = .75;}
    else if(intensity > .05){texColor.x = .35; texColor.y = .35; texColor.z = .35;}
    else{texColor.x = .25; texColor.y = .25; texColor.z = .25;}

 

}*/
