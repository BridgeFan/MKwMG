#version 420 core
layout( quads ) in;
//common uniforms
uniform mat4 view;
uniform mat4 projection;
//unique uniforms
uniform vec2 DivisionBegin;
uniform vec2 DivisionEnd;

//TODO - draw Bézier Surface C2

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
	gl_Position = projection * view * vec4(p, 1.0);
}
