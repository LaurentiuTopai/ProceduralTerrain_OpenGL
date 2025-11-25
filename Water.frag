#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform vec3 waterColor;

void main()
{
    FragColor = vec4(waterColor, 0.6); // alfa 0.6 pentru transparen??
}
