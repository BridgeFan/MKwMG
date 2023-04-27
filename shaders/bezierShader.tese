#version 420 core
layout( isolines ) in;
//common uniforms
uniform mat4 view;
uniform mat4 projection;
//unique uniforms
uniform int BezierNum;

vec3 deCasteljau(float t, vec3 pos[4], int N) {
    vec3 beta[4] = pos;
    for(uint i=1;i<=N;i++) {
        for(uint k=0;k<N-i;k++) {
                beta[k] = beta[k]*(1-t)+beta[k+1]*t;
            }
        }
    return beta[0];
}

layout( isolines ) in;
uniform mat4 MVP; // projection * view * model
void main() {
    // The tessellation u coordinate
    float u = gl_TessCoord.x;
    // The patch vertices (control points)
    vec3 points[4];
    for(int i=0;i<BezierNum;i++)
        points[i] = gl_in[i].gl_Position.xyz;
    vec3 p = deCasteljau(u,points,BezierNum);
    gl_Position = projection * view * vec4(p, 1.0);
}
