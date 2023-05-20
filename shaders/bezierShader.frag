#version 420 core
out vec4 FragColor;
//unique uniforms
uniform vec3 color;

void main()
{
    FragColor = vec4(color,1);
}
