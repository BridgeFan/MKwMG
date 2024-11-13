#version 420 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    FragColor = vec4(tex1.r, tex2.gb, 1.0);
}
