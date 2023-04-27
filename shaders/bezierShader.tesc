#version 420 core
layout( vertices=4 ) out;
//common uniforms
uniform mat4 view;
uniform mat4 projection;
//unique uniforms
uniform int MinSegments;
uniform int MaxSegments;
uniform int ScreenWidth;
uniform int ScreenHeight;
uniform int BezierNum;

vec2 toScreenPos(vec3 worldPos) {
    vec4 v = projection*view*vec4(worldPos,1);
    v/=v.w;
    v.x=(v.x+1.f)*ScreenWidth*.5f;
    v.y=(1.f-v.y)*ScreenHeight*.5f;
    return v.xy;
}

void main() {
    float dist=0;
    if(BezierNum>2) {
        vec2 p0 = toScreenPos(gl_in[gl_InvocationID].gl_Position.xyz);
        vec2 p1 = toScreenPos(gl_in[gl_InvocationID+1].gl_Position.xyz);
        vec2 p2 = toScreenPos(gl_in[gl_InvocationID+2].gl_Position.xyz);
        dist = distance(p0,p1);
        dist += distance(p1,p2);
        if(BezierNum==4) {
            vec2 p3 = toScreenPos(gl_in[gl_InvocationID+3].gl_Position.xyz);
            dist += distance(p2,p3);
        }
    }
    // Pass along the vertex position unmodified
    gl_out[gl_InvocationID].gl_Position =
    gl_in[gl_InvocationID].gl_Position;
    // Define the tessellation levels
    gl_TessLevelOuter[0] = float(1);
    int segNum = clamp(int(dist*4),MinSegments,MaxSegments);
    gl_TessLevelOuter[1] = float(int(segNum));
}