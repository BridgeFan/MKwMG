#version 420 core
layout (location = 0) in vec3 aPos;

out vec4 vertexColor;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0f);
    gl_PointSize = 16;
    vertexColor = vec4(color, 1.0);
}

