#version 420 core
out vec4 FragColor;

in vec4 vertexColor;

void main()
{
    FragColor = vec4(vec3(1.0,1.0,1.0)-vertexColor.xyz, vertexColor.a);
}
