#version 330 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPosWS;
out vec3 normalWS;
out vec2 fragUV; // schimb numele aici ca s? fie consistent cu fragment shader

void main()
{
    fragPosWS = vec3(model * vec4(inPosition, 1.0));
    normalWS = normalize(mat3(transpose(inverse(model))) * inNormal);
    fragUV = inTexCoord; // corect
    gl_Position = projection * view * vec4(fragPosWS, 1.0);
}
