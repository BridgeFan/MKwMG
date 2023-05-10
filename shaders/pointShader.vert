#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec4 vertexColor;
//common uniforms
uniform mat4 view;
uniform mat4 projection;
//unique uniforms
uniform vec3 color;
uniform vec3 position;
uniform float pointSize;

mat4 getMatrixFromPosition() {
    mat4 matrix = mat4(1.0);
    matrix[3]=vec4(position, 1.0);
    return matrix;
}

void main()
{
    gl_Position = projection * view * getMatrixFromPosition() * vec4(aPos, 1.0f);
    gl_PointSize = pointSize;
    vertexColor = vec4(color, 1.0);
}

