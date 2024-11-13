#version 420 core
out vec4 FragColor;
//unique uniforms
uniform vec3 color;
uniform sampler2D TEX;
uniform int textureMode;
in vec2 t;
void main()
{
    if(textureMode!=0) {
        //0 - mode 1
        //1 - mode 2
        //0.5 - line
        float r = texture(TEX,t).r;
        if(r>0.6 && textureMode==2)
            discard;
        if(r<0.4 && textureMode==1)
            discard;
    }
    FragColor = vec4(color,1);
}
