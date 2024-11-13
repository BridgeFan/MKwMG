#version 420 core
layout( vertices=20 ) out;
//common uniforms
uniform mat4 view;
uniform mat4 projection;
//unique uniforms
uniform int Segments;


void main() {
	// Pass along the vertex position unmodified
	gl_out[gl_InvocationID].gl_Position =
	gl_in[gl_InvocationID].gl_Position;
	
	gl_TessLevelOuter[0] = float(Segments);
	gl_TessLevelOuter[1] = float(Segments);
	gl_TessLevelOuter[2] = float(Segments);
	gl_TessLevelOuter[3] = float(Segments);
	
	gl_TessLevelInner[0] = float(Segments);
	gl_TessLevelInner[1] = float(Segments);
}
