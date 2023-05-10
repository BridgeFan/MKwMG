#version 420 core
layout( quads ) in;
//common uniforms
uniform mat4 view;
uniform mat4 projection;
//unique uniforms
uniform vec2 DivisionBegin;
uniform vec2 DivisionEnd;

vec2 lerp(float t) {
	return DivisionBegin+t*(DivisionEnd-DivisionBegin);
}

void basisFunctions(out float[4] b, float t) {
	float t1 = (1.0 - t);
	float t12 = t1 * t1;
	// Bernstein polynomials
	b[0] = t12 * t1;
	b[1] = 3.0 * t12 * t;
	b[2] = 3.0 * t1 * t * t;
	b[3] = t * t * t;
}

layout( quads ) in;
uniform mat4 MVP; // projection * view * model
void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	/*vec4 p00 = gl_in[0].gl_Position;
	vec4 p01 = gl_in[1].gl_Position;
	vec4 p02 = gl_in[2].gl_Position;
	vec4 p03 = gl_in[3].gl_Position;
	vec4 p10 = gl_in[4].gl_Position;
	vec4 p11 = gl_in[5].gl_Position;
	vec4 p12 = gl_in[6].gl_Position;
	vec4 p13 = gl_in[7].gl_Position;
	vec4 p20 = gl_in[8].gl_Position;
	vec4 p21 = gl_in[9].gl_Position;
	vec4 p22 = gl_in[10].gl_Position;
	vec4 p23 = gl_in[11].gl_Position;
	vec4 p30 = gl_in[12].gl_Position;
	vec4 p31 = gl_in[13].gl_Position;
	vec4 p32 = gl_in[14].gl_Position;
	vec4 p33 = gl_in[15].gl_Position;*/
	// Compute basis functions
	float bu[4], bv[4];
	 // Basis functions for u and v
	basisFunctions(bu, u);
	basisFunctions(bv, v);
	// Bezier interpolation
	vec3 p = vec3(0.0);
	for(int i=0;i<4;i++) {
		for(int j=0;j<4;j++) {
			p += gl_in[4*i+j].gl_Position.xyz*bu[i]*bv[j];
		}
	}
	/*vec3 p =
	p00*bu[0]*bv[0] + p01*bu[0]*bv[1] + p02*bu[0]*bv[2] +
	p03*bu[0]*bv[3] +
	p10*bu[1]*bv[0] + p11*bu[1]*bv[1] + p12*bu[1]*bv[2] +
	p13*bu[1]*bv[3] +
	p20*bu[2]*bv[0] + p21*bu[2]*bv[1] + p22*bu[2]*bv[2] +
	p23*bu[2]*bv[3] +
	p30*bu[3]*bv[0] + p31*bu[3]*bv[1] + p32*bu[3]*bv[2] +
	p33*bu[3]*bv[3];*/
	// The normal is the cross product of the partials
	gl_Position = projection * view * vec4(p, 1.0);
}
