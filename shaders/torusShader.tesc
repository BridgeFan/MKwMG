#version 420 core
layout( vertices=16 ) out;
//unique uniforms
uniform int SegmentsX;
uniform int SegmentsY;


void main() {
	// Pass along the vertex position unmodified
	gl_out[gl_InvocationID].gl_Position =
	gl_in[gl_InvocationID].gl_Position;
	
	gl_TessLevelOuter[0] = float(SegmentsY);
	gl_TessLevelOuter[1] = float(SegmentsX);
	gl_TessLevelOuter[2] = float(SegmentsY);
	gl_TessLevelOuter[3] = float(SegmentsX);
	
	gl_TessLevelInner[0] = float(SegmentsX);
	gl_TessLevelInner[1] = float(SegmentsY);
}
