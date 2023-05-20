#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec4 vertexColor;
//common uniforms
uniform mat4 view;
uniform mat4 projection;
//unique uniforms
uniform vec3 color;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vertexColor = vec4(color, 1.0);
}

